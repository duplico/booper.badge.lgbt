LENGTH = 90;
WIDTH = 6;
LINES = 5;
BASE = 25;
STAMP_HEIGHT = 1;

cube([90, WIDTH, BASE]);

for(i = [0 : LINES-1]) {
    translate([0, 0.25 * WIDTH/LINES + i * (WIDTH/LINES), BASE])
        cube([90, 0.5 * WIDTH/LINES, STAMP_HEIGHT]);
}