LENGTH = 90;
WIDTH = 12.7;
LINES = 6;
BASE = 35;
STAMP_HEIGHT = 1;

cube([90, WIDTH, BASE]);

for(i = [0 : LINES-1]) {
    translate([0, 0.25 * WIDTH/LINES + i * (WIDTH/LINES), BASE])
        cube([90, 0.5 * WIDTH/LINES, STAMP_HEIGHT]);
}