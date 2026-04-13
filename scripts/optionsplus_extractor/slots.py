"""Parse core_metadata.json image assignments into typed slot records."""

from __future__ import annotations

import re
from dataclasses import dataclass, field
from typing import Optional


# Virtual CID used for the thumb wheel. The HID++ device does not expose
# a real CID for the thumb wheel button, so we emit a synthetic 0x0000
# entry that the app recognizes as a sentinel (AppController.cpp:366).
THUMBWHEEL_CID = 0x0000


# Slot name → (defaultName, defaultActionType, configurable)
SLOT_NAME_MAP: dict[str, tuple[str, str, bool]] = {
    "SLOT_NAME_MIDDLE_BUTTON":       ("Middle click",     "default",           True),
    "SLOT_NAME_BACK_BUTTON":         ("Back",              "default",           True),
    "SLOT_NAME_FORWARD_BUTTON":      ("Forward",           "default",           True),
    "SLOT_NAME_GESTURE_BUTTON":      ("Gesture button",    "gesture-trigger",   True),
    "SLOT_NAME_DPI_BUTTON":          ("DPI button",        "default",           True),
    "SLOT_NAME_LEFT_SCROLL_BUTTON":  ("Shift wheel mode",  "smartshift-toggle", True),
    "SLOT_NAME_RIGHT_SCROLL_BUTTON": ("Shift wheel mode",  "smartshift-toggle", True),
    "SLOT_NAME_MODESHIFT_BUTTON":    ("Shift wheel mode",  "smartshift-toggle", True),
    "SLOT_NAME_SIDE_BUTTON_TOP":     ("Top button",        "default",           True),
    "SLOT_NAME_SIDE_BUTTON_BOTTOM":  ("Bottom button",     "default",           True),
    "SLOT_NAME_THUMBWHEEL":          ("Thumb wheel",       "default",           True),
    "SLOT_NAME_MISSION_CONTROL":     ("Mission Control",   "default",           True),
    "SLOT_NAME_EMOJI":               ("Emoji",             "default",           True),
    "SLOT_NAME_SMART_ZOOM":          ("Smart zoom",        "default",           True),
}


class UnknownSlotName(Exception):
    """Raised when a slot name isn't in SLOT_NAME_MAP."""


@dataclass
class ButtonSlot:
    cid: int
    name: str
    action_type: str
    configurable: bool
    x_pct: float
    y_pct: float


@dataclass
class ScrollSlot:
    kind: str        # "scrollwheel" | "thumbwheel" | "pointer"
    x_pct: float
    y_pct: float


@dataclass
class EasySwitchSlot:
    index: int       # 1-based
    x_pct: float
    y_pct: float


@dataclass
class ParsedMetadata:
    buttons: list[ButtonSlot] = field(default_factory=list)
    scroll: list[ScrollSlot] = field(default_factory=list)
    easyswitch: list[EasySwitchSlot] = field(default_factory=list)


def parse(metadata: Optional[dict]) -> ParsedMetadata:
    """Parse a core_metadata.json dict into typed slot records.

    Unknown slot names raise UnknownSlotName. Assignments with no
    recognizable CID or slot kind are silently skipped (not all
    slots in every image are ones we care about).
    """
    out = ParsedMetadata()
    if not metadata:
        return out

    images = metadata.get("images", []) or []
    for img in images:
        key = img.get("key")
        assignments = img.get("assignments", []) or []
        if key == "device_buttons_image":
            out.buttons.extend(_parse_buttons(assignments))
        elif key == "device_point_scroll_image":
            out.scroll.extend(_parse_scroll(assignments))
        elif key == "device_easyswitch_image":
            out.easyswitch.extend(_parse_easyswitch(assignments))
        # device_gesture_buttons_image is intentionally skipped
        # (default gestures come from AppController hardcoded defaults)

    return out


def _marker_to_pct(marker: dict) -> tuple[float, float]:
    """Options+ markers encode position as percentages in [0, 100]."""
    x = round(float(marker.get("x", 0)) / 100.0, 3)
    y = round(float(marker.get("y", 0)) / 100.0, 3)
    return max(0.0, min(1.0, x)), max(0.0, min(1.0, y))


_CID_SUFFIX_RE = re.compile(r"_c(\d+)$")


def _cid_from_slot_id(slot_id: str) -> Optional[int]:
    m = _CID_SUFFIX_RE.search(slot_id or "")
    return int(m.group(1)) if m else None


def _is_thumbwheel_slot(slot_id: str, slot_name: str) -> bool:
    return (
        slot_name == "SLOT_NAME_THUMBWHEEL"
        or "thumb_wheel" in (slot_id or "").lower()
    )


def _parse_buttons(assignments: list[dict]) -> list[ButtonSlot]:
    out: list[ButtonSlot] = []
    for a in assignments:
        slot_id = a.get("slotId", "") or ""
        slot_name = a.get("slotName", "") or ""

        cid = _cid_from_slot_id(slot_id)
        is_thumb = _is_thumbwheel_slot(slot_id, slot_name)
        if cid is None and not is_thumb:
            continue
        if cid is None and is_thumb:
            cid = THUMBWHEEL_CID

        if slot_name not in SLOT_NAME_MAP:
            raise UnknownSlotName(
                f"unknown slot name {slot_name!r} (slotId={slot_id!r})"
            )
        name, action_type, configurable = SLOT_NAME_MAP[slot_name]

        x, y = _marker_to_pct(a.get("marker", {}) or {})
        out.append(ButtonSlot(
            cid=cid,
            name=name,
            action_type=action_type,
            configurable=configurable,
            x_pct=x,
            y_pct=y,
        ))
    return out


def _scroll_kind_from_slot_id(slot_id: str) -> Optional[str]:
    sid = slot_id.lower()
    if "scroll_wheel" in sid:
        return "scrollwheel"
    if "thumb_wheel" in sid:
        return "thumbwheel"
    if "mouse_settings" in sid or "pointer" in sid:
        return "pointer"
    return None


def _parse_scroll(assignments: list[dict]) -> list[ScrollSlot]:
    out: list[ScrollSlot] = []
    for a in assignments:
        slot_id = a.get("slotId", "") or ""
        kind = _scroll_kind_from_slot_id(slot_id)
        if kind is None:
            # No recognized kind — skip rather than raise. Scroll-image
            # assignments are a known superset of what PointScrollPage
            # renders.
            continue
        x, y = _marker_to_pct(a.get("marker", {}) or {})
        out.append(ScrollSlot(kind=kind, x_pct=x, y_pct=y))
    return out


_EASYSWITCH_RE = re.compile(r"_easy_switch_(\d+)$")


def _parse_easyswitch(assignments: list[dict]) -> list[EasySwitchSlot]:
    out: list[EasySwitchSlot] = []
    for a in assignments:
        slot_id = a.get("slotId", "") or ""
        m = _EASYSWITCH_RE.search(slot_id)
        if not m:
            continue
        idx = int(m.group(1))
        x, y = _marker_to_pct(a.get("marker", {}) or {})
        out.append(EasySwitchSlot(index=idx, x_pct=x, y_pct=y))
    out.sort(key=lambda s: s.index)
    return out
