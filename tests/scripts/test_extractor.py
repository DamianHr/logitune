"""Tests for scripts/optionsplus_extractor/. See spec
docs/superpowers/specs/2026-04-13-optionsplus-extractor-design.md."""

import importlib


def test_package_imports():
    mod = importlib.import_module("optionsplus_extractor")
    assert mod.__doc__ is not None


def test_all_submodules_import():
    for name in ("sources", "capabilities", "slots", "canonicalize",
                 "descriptor", "validate", "cli"):
        importlib.import_module(f"optionsplus_extractor.{name}")


from pathlib import Path
from optionsplus_extractor import sources

FIXTURE_ROOT = Path(__file__).parent / "fixtures" / "optionsplus"


def test_load_device_db_returns_two_mice():
    mice = sources.load_device_db(FIXTURE_ROOT / "main" / "logioptionsplus")
    assert set(mice.keys()) == {"mx_master_2s", "mx_master_3s"}


def test_device_db_entry_has_expected_fields():
    mice = sources.load_device_db(FIXTURE_ROOT / "main" / "logioptionsplus")
    m3s = mice["mx_master_3s"]
    assert m3s.name == "MX Master 3S"
    # 3S has two hardware revisions with PIDs b034 and b043
    assert "0xb034" in m3s.product_ids
    assert isinstance(m3s.capabilities, dict)
    assert m3s.capabilities.get("hasHighResolutionSensor") is True


def test_load_depot_finds_metadata_and_images():
    depot = sources.load_depot(FIXTURE_ROOT / "devices" / "mx_master_3s")
    assert depot.metadata is not None
    assert "images" in depot.metadata
    assert depot.front_image is not None
    assert depot.front_image.exists()


def test_load_depot_handles_2s_bottom_image_as_back():
    # 2S has bottom_core.png as its back image fallback; 3S has back_core.png.
    # Both should resolve via the back_image field.
    depot_2s = sources.load_depot(FIXTURE_ROOT / "devices" / "mx_master_2s")
    depot_3s = sources.load_depot(FIXTURE_ROOT / "devices" / "mx_master_3s")
    assert depot_2s.back_image is not None and depot_2s.back_image.name == "bottom_core.png"
    assert depot_3s.back_image is not None and depot_3s.back_image.name == "back_core.png"


from optionsplus_extractor import capabilities


def test_features_for_mx_master_3s():
    mice = sources.load_device_db(FIXTURE_ROOT / "main" / "logioptionsplus")
    caps = mice["mx_master_3s"].capabilities
    f = capabilities.features_from_capabilities(caps)
    assert f["battery"] is True
    assert f["adjustableDpi"] is True
    assert f["smartShift"] is True
    assert f["hiResWheel"] is True
    assert f["thumbWheel"] is True
    assert f["reprogControls"] is True
    assert f["smoothScroll"] is True
    assert f["gestureV2"] is False
    assert f["hapticFeedback"] is False


def test_dpi_from_high_res_sensor_info():
    mice = sources.load_device_db(FIXTURE_ROOT / "main" / "logioptionsplus")
    caps = mice["mx_master_3s"].capabilities
    dpi = capabilities.dpi_from_capabilities(caps)
    assert dpi["min"] == 200
    assert dpi["max"] == 8000
    assert dpi["step"] == 50


def test_dpi_defaults_when_no_sensor_info():
    dpi = capabilities.dpi_from_capabilities({})
    assert dpi == {"min": 200, "max": 4000, "step": 50}


def test_features_default_false_on_empty_caps():
    f = capabilities.features_from_capabilities({})
    assert f["battery"] is False
    assert f["smartShift"] is False
    # smoothScroll DEFAULTS TO TRUE per parser convention
    assert f["smoothScroll"] is True


from optionsplus_extractor import slots


def test_parse_buttons_for_3s():
    depot = sources.load_depot(FIXTURE_ROOT / "devices" / "mx_master_3s")
    parsed = slots.parse(depot.metadata)
    assert len(parsed.buttons) == 6  # 6 configurable buttons on MX Master 3S
    cids = sorted(b.cid for b in parsed.buttons)
    # 3S has Middle(0x52), Back(0x53), Forward(0x56), Gesture(0xC3),
    # Shift(0xC4), Thumb(0x0000 synthetic)
    assert 0x0000 in cids  # thumbwheel synthetic
    assert 0x0052 in cids
    assert 0x00C3 in cids
    assert 0x00C4 in cids


def test_parse_scroll_for_3s():
    depot = sources.load_depot(FIXTURE_ROOT / "devices" / "mx_master_3s")
    parsed = slots.parse(depot.metadata)
    kinds = {s.kind for s in parsed.scroll}
    assert kinds == {"scrollwheel", "thumbwheel", "pointer"}


def test_parse_easyswitch_for_3s():
    depot = sources.load_depot(FIXTURE_ROOT / "devices" / "mx_master_3s")
    parsed = slots.parse(depot.metadata)
    assert len(parsed.easyswitch) == 3
    # slot numbers must be 1, 2, 3 in order
    assert [s.index for s in parsed.easyswitch] == [1, 2, 3]


def test_marker_coords_are_percentages_not_pixels():
    depot = sources.load_depot(FIXTURE_ROOT / "devices" / "mx_master_3s")
    parsed = slots.parse(depot.metadata)
    for b in parsed.buttons:
        assert 0.0 <= b.x_pct <= 1.0
        assert 0.0 <= b.y_pct <= 1.0
    # Middle click sits near the top of the scroll wheel — x and y
    # should be well above the origin, not a fraction-of-a-pixel away.
    middle = next(b for b in parsed.buttons if b.cid == 0x0052)
    assert middle.x_pct > 0.5
    assert middle.y_pct < 0.3
    assert middle.y_pct > 0.05  # not clustered at origin


def test_unknown_slot_name_raises():
    bogus = {
        "images": [{
            "key": "device_buttons_image",
            "origin": {"width": 100, "height": 100},
            "assignments": [{
                "slotId": "x_c82",
                "slotName": "SLOT_NAME_NONEXISTENT_BUTTON",
                "marker": {"x": 50, "y": 50},
            }],
        }],
    }
    import pytest
    with pytest.raises(slots.UnknownSlotName) as exc:
        slots.parse(bogus)
    assert "SLOT_NAME_NONEXISTENT_BUTTON" in str(exc.value)
