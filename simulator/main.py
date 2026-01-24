import pygame
from consts import *
from kin import *
import socket
from Keys import *

pygame.init()

efx, efy = 0, 0



s = socket.socket()

def main():
    s.bind((IP, PORT))
    s.listen(1)

    print(f"Listening on {IP}:{PORT}")
    conn, addr = s.accept()

    print(f"{addr[0]}:{addr[1]} has connected!")

    data = conn.recv(1024)
    print(data.decode())
    

    display = pygame.display.set_mode((WIDTH, HEIGHT))

    running = True

    s1, s2 = inv_kin(0, 15.3)
    clicked = False
    click_pos = None
    while running:
        display.fill("#ffffff")

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            
            if event.type == pygame.MOUSEBUTTONDOWN and not clicked:
                clicked = True
                click_pos = event.pos
        
        if clicked and click_pos:
            clicked = False
            
            click_x, click_y = click_pos

            click_x  = (click_x - CENTER_X) / SCALE
            click_y  = click_y / SCALE

            print(f"{click_x} {click_y}")
            conn.send(f"{click_x} {click_y}\n".encode())

            tmp1, tmp2 = inv_kin(click_x, click_y)

            # Check if tmp1 is not None (meaning a valid solution was found)
            if tmp1 is not None:
                s1, s2 = tmp1, tmp2

        # Motors Base
        pygame.draw.rect(display, "#808080", (CENTER_X - L0*SCALE, 0, 2*L0*SCALE, (L0/2)*SCALE))

        # Arms S->E
        left_e_pos = calc_elbow_pos(LEFT_MOTOR_POS, s1, L, left=True)
        right_e_pos = calc_elbow_pos(RIGHT_MOTOR_POS, s2, L)
        pygame.draw.line(display, "#ff0000", LEFT_MOTOR_POS, left_e_pos, width=4) # Left Arm
        pygame.draw.line(display, "#ff0000", RIGHT_MOTOR_POS, right_e_pos, width=4) # Right Arm

        # Arms E->EF
        efx, efy = forward_kin(s1, s2)

        efx *= SCALE
        efy *= SCALE

        efx += CENTER_X

        pygame.draw.line(display, "#ff0000", left_e_pos, (efx, efy), width=4) # Left Arm
        pygame.draw.line(display, "#ff0000", right_e_pos, (efx, efy), width=4) # Right Arm

        # Shoulders
        pygame.draw.circle(display, "#000000", LEFT_MOTOR_POS, (L0 /4)*SCALE) # Left Motor
        pygame.draw.circle(display, "#000000", RIGHT_MOTOR_POS, (L0 /4)*SCALE) # Right Motor

        # Elbows
        pygame.draw.circle(display, "#000000", left_e_pos, 0.5*SCALE) # Left Motor
        pygame.draw.circle(display, "#000000", right_e_pos, 0.5*SCALE) # Right Motor

        # EF
        pygame.draw.circle(display, "#000000", (efx, efy), 0.5*SCALE) # Right Motor

        pygame.display.flip()

    pygame.quit()

if __name__ == "__main__":
    main()
    s.close()