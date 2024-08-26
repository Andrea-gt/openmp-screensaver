import cairosvg
from PIL import Image
import numpy as np
import csv

def svg_to_png(svg_file, png_file):
    """Convierte un archivo SVG a PNG usando cairosvg"""
    cairosvg.svg2png(url=svg_file, write_to=png_file)

def extract_colors_from_image(png_file):
    """Extrae los colores de una imagen PNG y los guarda en un formato específico"""
    image = Image.open(png_file).convert('RGBA')
    np_image = np.array(image)

    width, height = image.size
    points = []

    for y in range(height):
        for x in range(width):
            rgba = np_image[y, x]
            r, g, b, a = rgba
            if a > 0:  # Considerar solo píxeles con opacidad mayor a 0
                opacity = a / 255.0
                points.append([x, y, r, g, b, round(opacity, 1)])  # Cambiado a una lista de valores para CSV

    return points

def save_points_to_csv(points, csv_file):
    """Guarda los puntos en un archivo CSV"""
    with open(csv_file, 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['x', 'y', 'r', 'g', 'b', 'opacity'])  # Encabezados de columna
        writer.writerows(points)

def main():
    svg_file = 'input.svg'
    png_file = 'dvd-logo-3.png'
    csv_file = 'output.csv'
    
    # Convertir SVG a PNG
    #svg_to_png(svg_file, png_file)
    
    # Extraer colores y puntos de la imagen
    points = extract_colors_from_image(png_file)
    
    # Guardar los puntos en un archivo CSV
    save_points_to_csv(points, csv_file)
    print(f'Data saved to {csv_file}')

if __name__ == '__main__':
    main()
