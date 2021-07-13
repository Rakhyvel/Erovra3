from PIL import Image
import colorsys

im = Image.open('c:/users/joseph/pictures/earth.png')
water_csv = open("c:/users/joseph/documents/water_color.csv", "w")
land_csv = open("c:/users/joseph/documents/land_color.csv", "w")
pix = im.load()
for x in range(im.size[0]):
    for y in range(im.size[1]):
        rgb_color = pix[x, y]
        hsv_color = colorsys.rgb_to_hsv(rgb_color[0], rgb_color[1], rgb_color[2])
        hue = hsv_color[0]
        saturation = hsv_color[1]
        value = hsv_color[2]
        if value > 0:
            if hue >= 0.5 and hue < 0.75:
                water_csv.write(str(hue) + "," + str(saturation) + "," + str(value) + "\n")
            elif hue < 0.5:
                land_csv.write(str(hue) + "," + str(saturation) + "," + str(value) + "\n")
water_csv.close()
land_csv.close()