import numpy as np
from PIL import Image, ImageDraw


class Canvas:
    def __init__(self, width, height, bg=(0, 0, 0)):
        self.width = width
        self.height = height
        self._image = Image.new("RGB", (width, height), bg)
        self._draw = ImageDraw.Draw(self._image)

    def clear(self, color=(0, 0, 0)):
        self._draw.rectangle([0, 0, self.width, self.height], fill=color)

    def rect(self, x, y, w, h, color):
        self._draw.rectangle([x, y, x + w, y + h], fill=color)

    def line(self, points, color, width=1):
        self._draw.line(list(points), fill=color, width=width)

    def circle(self, x, y, r, color):
        self._draw.ellipse([x - r, y - r, x + r, y + r], fill=color)

    def image(self, grid, x=0, y=0):
        grid = np.clip(grid, 0.0, 1.0)
        pixels = (grid * 255).astype(np.uint8)
        tile = Image.fromarray(pixels, mode="L").convert("RGB")
        self._image.paste(tile, (x, y))

    def frame(self):
        return np.array(self._image)

    def save(self, path):
        self._image.save(path)
