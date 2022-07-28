import serial
import pygame

pygame.init()
pygame.font.init()

WIDTH = 800
HEIGHT = 800

screen = pygame.display.set_mode([WIDTH, HEIGHT])

bg = pygame.image.load("SnakeTitle.png")
font = pygame.font.SysFont("Comic sans", 30)

pygame.display.set_caption("Snake Controller")
pygame.display.set_icon(bg)

arduino = serial.Serial("COM3", 9600)


def draw():
    screen.fill(pygame.Color("lightgreen"))
    screen.blit(bg, (WIDTH / 2 - bg.get_width() / 2, 0))
    text1 = font.render("Press SPACE or ENTER to start",
                        True, pygame.Color("black"))
    text2 = font.render("Control the snake with W/A/S/D",
                        True, pygame.Color("black"))
    text3 = font.render("Press SHIFT to change the speed",
                        True, pygame.Color("black"))
    text4 = font.render("Press TAB to change the theme",
                        True, pygame.Color("black"))
    text5 = font.render("Press H to see the highscore",
                        True, pygame.Color("black"))
    text6 = font.render("Press ESC to quit", True, pygame.Color("black"))
    screen.blit(text1, (WIDTH / 2 - text1.get_width() / 2, bg.get_height()))
    screen.blit(text2, (WIDTH / 2 - text2.get_width() /
                2, bg.get_height() + text1.get_height()))
    screen.blit(text3, (WIDTH / 2 - text3.get_width() / 2,
                bg.get_height() + text1.get_height() * 2))
    screen.blit(text4, (WIDTH / 2 - text4.get_width() / 2,
                bg.get_height() + text1.get_height() * 3))
    screen.blit(text5, (WIDTH / 2 - text5.get_width() / 2,
                bg.get_height() + text1.get_height() * 4))
    screen.blit(text6, (WIDTH / 2 - text6.get_width() / 2,
                bg.get_height() + text1.get_height() * 5))
    pygame.display.flip()


def switchIdleMode():
    arduino.write(b"switchIdleMode#")
    print("switchIdleMode")


running = True
switchIdleMode()
while running:
    draw()
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_w or event.key == pygame.K_UP:
                arduino.write(b"up#")
                print("up")
            if event.key == pygame.K_s or event.key == pygame.K_DOWN:
                arduino.write(b"down#")
                print("down")
            if event.key == pygame.K_a or event.key == pygame.K_LEFT:
                arduino.write(b"left#")
                print("left")
            if event.key == pygame.K_d or event.key == pygame.K_RIGHT:
                arduino.write(b"right#")
                print("right")
            if event.key == pygame.K_TAB:
                arduino.write(b"theme#")
                print("theme")
            if event.key == pygame.K_LSHIFT:
                arduino.write(b"speed#")
                print("speed")
            if event.key == pygame.K_h:
                arduino.write(b"highscore#")
                print("highscore")
            if event.key == pygame.K_SPACE or event.key == pygame.K_RETURN:
                arduino.write(b"start#")
                print("start")
            if event.key == pygame.K_ESCAPE:
                arduino.write(b"escape#")
                print("escape")

pygame.quit()
switchIdleMode()