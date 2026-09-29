#include <iostream>
#include <optional>
#include <vector>
#include <cmath>

#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int FPS_LIMIT = 30;
int NUM_POINTS = 4;

using Point2D = sf::Vector2f;

// (Part 1) Define a function that samples a cubic Bezier curve at t in [0, 1].
Point2D getPoint(const std::vector<sf::Vector2f>& pts, float t) {
    return (1-t)*(1-t)*(1-t)*pts[0] + 3*(1-t)*(1-t)*t*pts[1] + 3*(1-t)*t*t*pts[2] + t*t*t*pts[3];
}

// (Part 2) Define a function that returns the curve's slope at t in [0, 1].
sf::Vector2f subtractVectors(sf::Vector2f a, sf::Vector2f b) {
    sf::Vector2f out;
    out.x = a.x - b.x;
    out.y = a.y - b.y;
    return out;
}
Point2D getSlope(const std::vector<sf::Vector2f>& pts, float t) { 
    return (3*(1-t)*(1-t)*(subtractVectors(pts[1],pts[0]))) + (6*(1-t)*t*(subtractVectors(pts[2],pts[1]))) + (3*t*t*(subtractVectors(pts[3],pts[2])));
}

// (Part 1) Store four control points for the curve.
std::vector<sf::Vector2f> myPoints{{100.0f, 100.0f}, {700.0f, 900.0f}, {80.0f, 700.0f}, {700.0f, 100.0f}};

// (Part 2) Track animation time for the square moving along the curve.
float squareT = 0.0f;
const float SQUARE_SIZE = 20.0f;

// (Part 3) Track the index of the control point being dragged.
int controlIndex = 0;
bool dragging = false;

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            // (Part 3) On left-click, select the closest control point
            sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition());
            float minDist = INFINITY;
            int closeIndex = 0;
            for (int i = 0; i < NUM_POINTS; ++i) {
                float curDist = std::abs(std::hypot(myPoints[i].x - mousePos.x, myPoints[i].y - mousePos.y));
                if (curDist < minDist) {
                    minDist = curDist;
                    closeIndex = i;
                }
            }
            controlIndex = closeIndex;
            // using mouse->position and start dragging it.
            dragging = true;
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
            // (Part 3) On left-button release, stop dragging.
            dragging = false;
        } else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>()) {
            // (Part 3) Move the selected control point to mouse->position.
            if (dragging) {
                myPoints[controlIndex] = static_cast<sf::Vector2f>(sf::Mouse::getPosition());
            }
            // TODO: (Part 4) Maintain matching slopes at shared endpoints.
            // When moving point 3, move point 5 without changing its distance
            // from point 4 (point numbers here start at 1).
        } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            // TODO: (Part 4) '+' adds three control points; '-' removes three,
            // keeping at least four points.
        }
    }
}

void render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    // ====== ====== ======
    // (Part 1) Sample GetPoint over t in [0, 1] and connect samples using the line-drawing
    // code from your project. Draw all four control points as circles after drawing the curve.
    // ====== ====== ======
    sf::VertexArray myCurve(sf::PrimitiveType::LineStrip, 0);
    for (float t = 0.0f; t < 1.0f; t += 0.01f) {
        Point2D curPoint = getPoint(myPoints, t);
        myCurve.append(sf::Vertex{{curPoint.x, curPoint.y}});
    }
    window.draw(myCurve);

    // ====== ====== ======
    // (Part 2) Draw a small square moving repeatedly along the curve.
    // Use GetSlope to orient it to the curve at each time step.
    // ====== ====== ======
    Point2D squarePoint = getPoint(myPoints, squareT);
    sf::RectangleShape mySquare(sf::Vector2f{SQUARE_SIZE, SQUARE_SIZE});
    mySquare.setPosition(sf::Vector2f{squarePoint.x, squarePoint.y});
    Point2D squareSlope = getSlope(myPoints, squareT);
    mySquare.setRotation(sf::degrees(atan2(squareSlope.y, squareSlope.x) * 180 / M_PI));
    mySquare.setFillColor(sf::Color::Cyan);
    window.draw(mySquare);

    if (squareT >= 1.0f) { squareT = 0.0f; }
    else { squareT += 0.01f; }


    // ====== ====== ======
    // (Part 3) Draw control handles from point 1 to 2 and point 3 to 4.
    // TODO: (Part 4) Draw all connected cubic Bezier segments and their handles.
    // ====== ====== ======
    sf::VertexArray curve12(sf::PrimitiveType::LineStrip, 2);
    curve12[0].position = sf::Vector2f{myPoints[0]};
    curve12[1].position = sf::Vector2f{myPoints[1]};
    curve12[0].color = sf::Color::Yellow;
    curve12[1].color = sf::Color::Blue;
    sf::VertexArray curve34(sf::PrimitiveType::LineStrip, 2);
    curve34[0].position = sf::Vector2f{myPoints[2]};
    curve34[1].position = sf::Vector2f{myPoints[3]};
    curve34[0].color = sf::Color::Blue;
    curve34[1].color = sf::Color::Yellow;
    window.draw(curve12);
    window.draw(curve34);


    // ====== ====== ======
    // TODO: (Bonus) Support multiple curves, a Galaga screen overlay at a 1:2 ratio, and exporting
    // curve points as C++ code for Project 1b.
    // ====== ====== ======

    window.display();
}

int main() {
    sf::RenderWindow window;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Bezier Curve Editor");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
