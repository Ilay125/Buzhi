import math
from consts import *

def calc_elbow_pos(s_pos, s_angle, arm_len, scale=SCALE, left=False):
    x, y = s_pos
    dx = math.cos(math.radians(s_angle)) * arm_len
    dy = math.sin(math.radians(s_angle)) * arm_len
    return x + dx * scale, y + dy * scale

def inv_kin(x, y):
    # Distance from each motor to the target (x,y)
    d1 = math.sqrt((x + L0 / 2)**2 + y**2)
    d2 = math.sqrt((x - L0 / 2)**2 + y**2)

    # Reachability Check
    if d1 > 2*L or d2 > 2*L:
        return None, None # Target is too far
    
    # Base angles (angle from motor to target)
    b1 = math.atan2(y, x + (L0/2))
    b2 = math.atan2(y, x - (L0/2))

    # Internal angles (Law of Cosines)
    c1 = d1 / (2*L)
    c2 = d2 / (2*L)

    if abs(c1) > 1 or abs(c2) > 1:
        return None, None # Geometry impossible
    
    a1 = math.acos(c1)
    a2 = math.acos(c2)

    # Calculate motor angles ("Elbows Out" Configuration)
    # Left Arm: Angle to target + Angle of triangle (Points Left/Out)
    theta1 = b1 + a1 
    # Right Arm: Angle to target - Angle of triangle (Points Right/Out)
    theta2 = b2 - a2

    return math.degrees(theta1), math.degrees(theta2)

def forward_kin(s1, s2):
    s1, s2 = math.radians(s1), math.radians(s2)

    # Calculate Elbow Positions
    xL = -(L0 / 2) + L * math.cos(s1)
    yL = L * math.sin(s1)
    xR = (L0 / 2) + L * math.cos(s2)
    yR = L * math.sin(s2)

    # Circle Intersection Math
    dx = xR - xL
    dy = yR - yL
    d = math.sqrt(dx**2 + dy**2)

    if d == 0 or d > 2*L:
        return 0, 0 

    a = d / 2
    h_val = L**2 - a**2
    if h_val < 0: h_val = 0
    h = math.sqrt(h_val)

    x0 = xL + a * dx / d
    y0 = yL + a * dy / d

    # Calculate intersection offsets
    rx = -h * dy / d
    ry = h * dx / d

    # Two possible intersection points
    x_opt1, y_opt1 = x0 + rx, y0 + ry
    x_opt2, y_opt2 = x0 - rx, y0 - ry

    # Pick the intersection with the Higher Y (Lower on screen)
    if y_opt1 > y_opt2:
        return x_opt1, y_opt1
    else:
        return x_opt2, y_opt2