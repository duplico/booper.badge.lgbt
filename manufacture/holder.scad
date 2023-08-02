OUTLINE_WIDTH = 85;
OUTLINE_HEIGHT = 120;
OUTLINE_THICKNESS = 5;

difference() {
    translate([-OUTLINE_WIDTH/2, -OUTLINE_HEIGHT/2, 0])
        cube([OUTLINE_WIDTH, OUTLINE_HEIGHT, OUTLINE_THICKNESS]);
    translate([0,0,-1]) rotate([0,180,0]) scale([0.1, 0.1, 5]) import("PCB1_repaired_fixed.stl",);
    translate([-OUTLINE_WIDTH/2,-75,0]) cube([OUTLINE_WIDTH, OUTLINE_HEIGHT/2, OUTLINE_THICKNESS]);
    translate([-5,36,0]) cube([10,10,10]);
}