#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>

void drawPixel(sf::RenderTarget& target, int x, int y, sf::Color color);

void drawLineNaiveLow(sf::RenderTarget& target, int x1, int y1, int x2, int y2, sf::Color startColor, sf::Color endColor);

void drawLineNaiveHigh(sf::RenderTarget& target, int x1, int y1, int x2, int y2, sf::Color startColor, sf::Color endColor);

void drawLine(sf::RenderTarget& target, int x1, int y1, int x2, int y2, sf::Color startColor, sf::Color endColor);

sf::Color interpolateColor(sf::Color beg, sf::Color fin, float inter);

void drawLineBresenham(sf::RenderTarget& target, int x0, int y0, int x1, int y1, sf::Color color);

void drawLineAA(sf::RenderTarget& target, int x0, int y0, int x1, int y1, sf::Color color);

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 800), "SFML Window");
    sf::Event event;

    sf::Color red(255, 0, 0);
    sf::Color green(0, 255, 0);
    sf::Color blue(0, 0, 255);
    sf::Color white(255, 255, 255);

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        drawLineNaiveLow(window, 50, 50, 250, 200, red, blue);
        drawLineNaiveHigh(window, 50, 50, 200, 250, green, blue);

        drawLine(window, 300, 50, 500, 200, red, blue);
        drawLine(window, 300, 50, 450, 250, green, blue);

        drawLineBresenham(window, 50, 300, 250, 450, red);
        drawLineBresenham(window, 50, 300, 200, 500, green);

        drawLineAA(window, 300, 300, 500, 450, white);

        window.display();
    }

    return 0;
}

void drawPixel(sf::RenderTarget& target, int x, int y, sf::Color color) {
    auto pixel = sf::RectangleShape(sf::Vector2f(1, 1));
    pixel.setFillColor(color);
    pixel.setPosition(sf::Vector2f(x, y));
    target.draw(pixel);
}

void drawLineNaiveLow(sf::RenderTarget& target, int x1, int y1, int x2, int y2, sf::Color startColor, sf::Color endColor) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int xi = 1;

    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }

    int D = (2 * dy) - dx;
    int y = y1;

    for (int x = x1; x <= x2; x++) {
        float inter = static_cast<float>(x - x1) / static_cast<float>(x2 - x1);
        sf::Color color = interpolateColor(startColor, endColor, inter);
        drawPixel(target, x, y, color);

        if (D > 0) {
            y = y + 1;
            D = D + (2 * (dy - dx));
        }
        else {
            D = D + 2 * dy;
        }
    }
}

void drawLineNaiveHigh(sf::RenderTarget& target, int x1, int y1, int x2, int y2, sf::Color startColor, sf::Color endColor) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int yi = 1;

    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }

    int D = (2 * dx) - dy;
    int x = x1;

    for (int y = y1; y <= y2; y++) {
        float inter = static_cast<float>(y - y1) / static_cast<float>(y2 - y1);
        sf::Color color = interpolateColor(startColor, endColor, inter);
        drawPixel(target, x, y, color);

        if (D > 0) {
            x = x + 1;
            D = D + (2 * (dx - dy));
        }
        else {
            D = D + 2 * dx;
        }
    }
}

void drawLine(sf::RenderTarget& target, int x1, int y1, int x2, int y2, sf::Color startColor, sf::Color endColor) {
    float slope = static_cast<float>(y2 - y1) / static_cast<float>(x2 - x1);

    if (std::abs(slope) <= 1.0f) {
        drawLineNaiveLow(target, x1, y1, x2, y2, startColor, endColor);
    }
    else {
        drawLineNaiveHigh(target, x1, y1, x2, y2, startColor, endColor);
    }
}

sf::Color interpolateColor(sf::Color beg, sf::Color fin, float inter) {
    int r = static_cast<int>(beg.r + (fin.r - beg.r) * inter);
    int g = static_cast<int>(beg.g + (fin.g - beg.g) * inter);
    int b = static_cast<int>(beg.b + (fin.b - beg.b) * inter);

    return sf::Color(r, g, b);
}

void drawLineBresenham(sf::RenderTarget& target, int x0, int y0, int x1, int y1, sf::Color color) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        drawPixel(target, x0, y0, color);

        if (x0 == x1 && y0 == y1) {
            break;
        }

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (x0 == x1 && y0 == y1) {
            drawPixel(target, x0, y0, color);
            break;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void drawLineAA(sf::RenderTarget& target, int x0, int y0, int x1, int y1, sf::Color color) {
    auto plot = [&](int x, int y, float brightness) {
        int r = static_cast<int>(color.r * brightness);
        int g = static_cast<int>(color.g * brightness);
        int b = static_cast<int>(color.b * brightness);
        drawPixel(target, x, y, sf::Color(r, g, b));
        };

    auto ipart = [](float x) -> int {
        return static_cast<int>(std::floor(x));
        };

    auto fpart = [](float x) -> float {
        return x - std::floor(x);
        };

    auto rfpart = [=](float x) -> float {
        return 1.0f - fpart(x);
        };

    bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    float gradient = (dx != 0) ? static_cast<float>(dy) / static_cast<float>(dx) : 1.0f;

    int xEnd = static_cast<int>(std::round(x0));
    float yEnd = y0 + gradient * (xEnd - x0);
    float xGap = rfpart(x0 + 0.5f);
    int xPixel1 = xEnd;
    int yPixel1 = ipart(yEnd);

    if (steep) {
        plot(yPixel1, xPixel1, rfpart(yEnd) * xGap);
        plot(yPixel1 + 1, xPixel1, fpart(yEnd) * xGap);
    }
    else {
        plot(xPixel1, yPixel1, rfpart(yEnd) * xGap);
        plot(xPixel1, yPixel1 + 1, fpart(yEnd) * xGap);
    }

    float intery = yEnd + gradient;

    xEnd = static_cast<int>(std::round(x1));
    yEnd = y1 + gradient * (xEnd - x1);
    xGap = fpart(x1 + 0.5f);
    int xPixel2 = xEnd;
    int yPixel2 = ipart(yEnd);

    if (steep) {
        plot(yPixel2, xPixel2, rfpart(yEnd) * xGap);
        plot(yPixel2 + 1, xPixel2, fpart(yEnd) * xGap);
    }
    else {
        plot(xPixel2, yPixel2, rfpart(yEnd) * xGap);
        plot(xPixel2, yPixel2 + 1, fpart(yEnd) * xGap);
    }

    if (steep) {
        for (int x = xPixel1 + 1; x < xPixel2; x++) {
            plot(ipart(intery), x, rfpart(intery));
            plot(ipart(intery) + 1, x, fpart(intery));
            intery += gradient;
        }
    }
    else {
        for (int x = xPixel1 + 1; x < xPixel2; x++) {
            plot(x, ipart(intery), rfpart(intery));
            plot(x, ipart(intery) + 1, fpart(intery));
            intery += gradient;
        }
    }
}
