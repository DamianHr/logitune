#!/usr/bin/env python3
"""
Generate a labeled debug image for G502 hotspot coordinate mapping.

Draws each button's index number and name at its current xPct/yPct position
on the front-view PNG. Open the output image, find where each label should
actually be, note the pixel position, divide by image dimensions to get xPct/yPct.

Usage:
    python3 scripts/gen_g502_hotspot_debug.py

Output:
    src/app/qml/assets/g502-proteus-spectrum-debug.png
"""

import sys
import os

try:
    from PIL import Image, ImageDraw, ImageFont
except ImportError:
    print("Pillow not installed. Run: pip install Pillow")
    sys.exit(1)

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT  = os.path.dirname(SCRIPT_DIR)
SRC_IMAGE  = os.path.join(REPO_ROOT, "src/app/qml/assets/g502-proteus-spectrum.png")
OUT_IMAGE  = os.path.join(REPO_ROOT, "src/app/qml/assets/g502-proteus-spectrum-debug.png")

# Current hotspot definitions from G502ProtusSpectrumDescriptor.cpp
# Format: (buttonIndex, xPct, yPct, name)
# Scroll hotspots use negative indices: -1=main wheel
BUTTON_HOTSPOTS = [
    (2,  0.50, 0.50, "Middle click"),
    (3,  0.50, 0.50, "Back"),
    (4,  0.50, 0.50, "Forward"),
    (5,  0.50, 0.50, "Scroll left tilt"),
    (6,  0.50, 0.50, "Scroll right tilt"),
    (7,  0.50, 0.50, "Sniper button"),
    (8,  0.50, 0.50, "DPI button"),
    (9,  0.50, 0.50, "Shift wheel mode"),
]

SCROLL_HOTSPOTS = [
    (-1, 0.50, 0.50, "Main scroll wheel"),
]

# Spread the labels out vertically so they don't overlap at center
# Each entry gets a unique y offset for readability
SPREAD_LAYOUT = [
    (2,  0.60, 0.10, "Middle click"),
    (3,  0.60, 0.20, "Back"),
    (4,  0.60, 0.30, "Forward"),
    (5,  0.60, 0.40, "Scroll left tilt"),
    (6,  0.60, 0.50, "Scroll right tilt"),
    (7,  0.60, 0.60, "Sniper button"),
    (8,  0.60, 0.70, "DPI button"),
    (9,  0.60, 0.80, "Shift wheel mode"),
    (-1, 0.60, 0.90, "Main scroll wheel"),
]

DOT_RADIUS   = 12
FONT_SIZE    = 18
LABEL_OFFSET = 16   # px to the right of the dot centre


def main():
    if not os.path.exists(SRC_IMAGE):
        print(f"Source image not found: {SRC_IMAGE}")
        print("Replace the placeholder PNG with a real G502 image first.")
        sys.exit(1)

    img = Image.open(SRC_IMAGE).convert("RGBA")
    w, h = img.size

    if w <= 1 or h <= 1:
        print(f"Source image is a 1x1 placeholder ({SRC_IMAGE}).")
        print("Replace it with a real G502 PNG, then re-run this script.")
        sys.exit(1)

    overlay = Image.new("RGBA", img.size, (0, 0, 0, 0))
    draw    = ImageDraw.Draw(overlay)

    try:
        font       = ImageFont.truetype("/usr/share/fonts/TTF/DejaVuSans-Bold.ttf", FONT_SIZE)
        font_small = ImageFont.truetype("/usr/share/fonts/TTF/DejaVuSans.ttf", 14)
    except OSError:
        try:
            font       = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", FONT_SIZE)
            font_small = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14)
        except OSError:
            font       = ImageFont.load_default()
            font_small = font

    colors = [
        (255,  80,  80, 220),  # red
        (255, 160,  50, 220),  # orange
        (255, 220,  50, 220),  # yellow
        ( 80, 200,  80, 220),  # green
        ( 50, 180, 255, 220),  # blue
        (160,  80, 255, 220),  # purple
        (255,  80, 200, 220),  # pink
        ( 80, 220, 180, 220),  # teal
        (220, 220, 220, 220),  # grey  (scroll)
    ]

    for i, (idx, xp, yp, name) in enumerate(SPREAD_LAYOUT):
        cx = int(xp * w)
        cy = int(yp * h)
        color = colors[i % len(colors)]
        label = f"[{idx}] {name}"

        # Dot
        draw.ellipse(
            [cx - DOT_RADIUS, cy - DOT_RADIUS, cx + DOT_RADIUS, cy + DOT_RADIUS],
            fill=color, outline=(255, 255, 255, 255), width=2
        )
        # Index number inside dot
        draw.text((cx, cy), str(idx), font=font_small, fill=(255, 255, 255, 255), anchor="mm")

        # Label pill
        bbox  = draw.textbbox((0, 0), label, font=font)
        tw    = bbox[2] - bbox[0]
        th    = bbox[3] - bbox[1]
        lx    = cx + DOT_RADIUS + LABEL_OFFSET
        ly    = cy - th // 2

        draw.rounded_rectangle(
            [lx - 4, ly - 2, lx + tw + 4, ly + th + 2],
            radius=4, fill=(0, 0, 0, 180)
        )
        draw.text((lx, ly), label, font=font, fill=(255, 255, 255, 255))

    # Draw coordinate grid lines at 25% intervals for reference
    for pct in [0.25, 0.50, 0.75]:
        x = int(pct * w)
        y = int(pct * h)
        draw.line([(x, 0), (x, h)], fill=(255, 255, 255, 40), width=1)
        draw.line([(0, y), (w, y)], fill=(255, 255, 255, 40), width=1)
        draw.text((x + 3, 3), f"{int(pct*100)}%", font=font_small, fill=(255, 255, 255, 120))

    # Watermark
    draw.text((8, h - 24), "LOGITUNE HOTSPOT DEBUG — replace placeholder image to use",
              font=font_small, fill=(255, 200, 50, 180))

    result = Image.alpha_composite(img, overlay)
    result.save(OUT_IMAGE)
    print(f"Written: {OUT_IMAGE}")
    print(f"Image size: {w}x{h}px")
    print()
    print("How to use:")
    print("  1. Open the debug image in an image editor")
    print("  2. For each button, find where its dot SHOULD be on the mouse body")
    print("  3. Note the pixel coordinates (px, py)")
    print(f"  4. Compute: xPct = px / {w},  yPct = py / {h}")
    print("  5. Update buttonHotspots() in G502ProtusSpectrumDescriptor.cpp")
    print("  6. Re-run this script to verify the new positions")


if __name__ == "__main__":
    main()
