#include <vector>
#include <cmath>
#include <cstdlib>
#include <SFML/Graphics.hpp>

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float density = 0.0f;
};

const float WIDTH = 1280.0f;
const float HEIGHT = 720.0f;
 
const int NUM_PARTICLES = 500;

const float GRAVITY = 0.0f;
const float PI = 3.14159265f;

const float PARTICLE_RADIUS = 5.0f;
const float SMOOTHING_RADIUS = 20.0f;

const float COLLISION_DAMP = 0.1f;

const float TARGET_DENSITY = 0.0001f; // How spaced out the fluid naturally wants to be
const float PRESSURE_MULTIPLIER = 70000.0f; // How agressively it fights being squished
const float VISCOSITY_STRENGTH = 1.0f;

const float MAX_SPEED = 100.0f;

float length(const sf::Vector2f& vector) {
    return sqrt(vector.x * vector.x + vector.y * vector.y);
}

float smoothingKernel(float dst, float radius) {
    if (dst >= radius) { return 0.0f; }
    float volume = PI * std::pow(radius, 4.0f);
    return (6.0f  / volume) * std::pow(radius - dst, 2);
}

// this function calculates the physical force, for that
// we need the derivative of the smoothing kernel
// for the slope of the curve ( bigger slope -> strong pushing force)
float smoothingKernelDerivative(float dst, float radius) {
    if (dst >= radius) { return 0.0f; }
    float volume = PI * std::pow(radius, 4.0f);
    return  (12.0f / volume) * (radius - dst);
}

// calculate the pressure given the particle density
float densityToPressure(float density) {
    float densityError = density - TARGET_DENSITY;
    return std::max(0.0f, densityError * PRESSURE_MULTIPLIER);
}

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Fluid Simulation");

    std::vector<Particle> particles;
    sf::CircleShape render_shape(PARTICLE_RADIUS);
    render_shape.setFillColor(sf::Color::Black);
    render_shape.setOrigin(PARTICLE_RADIUS, PARTICLE_RADIUS);

    int cols = 25;
    float spacing = 2 * PARTICLE_RADIUS + 2.f;
    float startX = 100.f;
    float startY = 100.f;

    for (int i = 0 ; i < NUM_PARTICLES ; ++i) {
        int x = i % cols;

        int y = i / cols;

        float jitterX = ((rand() % 100) / 100.0f) - 2.0f;
        float jitterY = ((rand() % 100) / 100.0f) - 2.0f;
        
        float posX = startX + (x * spacing) + jitterX;
        float posY = startY + (y * spacing) + jitterY;

        Particle p;
        p.position.x = posX; p.position.y = posY;
        p.velocity.x = 0.0f; p.velocity.y = 0.0f;
        particles.push_back(p);
    }
    


    sf::Clock clock;
    while(window.isOpen()) {

        float dt = clock.restart().asSeconds();

        if (dt > 0.016f) {
            dt = 0.016f;
        }

        // density calculation
        for (int i = 0 ; i < NUM_PARTICLES ; i++) {
            particles[i].density = 0.0f;
            for (int j = 0 ; j < NUM_PARTICLES ; j++) {
                float dst = length(particles[i].position - particles[j].position);
                float kernel = smoothingKernel(dst, SMOOTHING_RADIUS);
                particles[i].density += kernel;
            }
        }

        // pressure / force loop
        for (int i = 0 ; i < NUM_PARTICLES ; i++) {
            float pressure_i = densityToPressure(particles[i].density);

            sf::Vector2f pressureForce(0.0f, 0.0f);
            sf::Vector2f viscosityForce(0.0f, 0.0f);

            for (int j = 0 ; j < NUM_PARTICLES ; j++) {
                if (i == j) { continue; }

                sf::Vector2f offset = particles[j].position - particles[i].position;
                float dst = length(offset);

                if (dst < SMOOTHING_RADIUS && dst > 0.001f) {
                    sf::Vector2f dir = offset / dst;

                    float slope = smoothingKernelDerivative(dst, SMOOTHING_RADIUS);
                    float pressure_j = densityToPressure(particles[j].density);
                    float sharedPressure = (pressure_i + pressure_j) / (2.0f * particles[j].density);


                    pressureForce -= dir * slope * sharedPressure;

                    float kernel = smoothingKernel(dst, SMOOTHING_RADIUS);
                    viscosityForce += (particles[j].velocity - particles[i].velocity) * kernel * VISCOSITY_STRENGTH;
                }
            }
            // F = m * a ----> a = F / density
            sf::Vector2f acceleration = (pressureForce + viscosityForce) / particles[i].density;
            particles[i].velocity += acceleration * dt;
        }

        for (auto& p : particles) {
            p.velocity.y += GRAVITY * dt;
            p.position.x += p.velocity.x * dt;
            p.position.y += p.velocity.y * dt;

            // Left wall
            if (p.position.x < PARTICLE_RADIUS) {
                p.position.x = PARTICLE_RADIUS;
                p.velocity.x *= -COLLISION_DAMP;
            }
            // Right wall
            if (p.position.x > WIDTH - PARTICLE_RADIUS) {
                p.position.x = WIDTH - PARTICLE_RADIUS;
                p.velocity.x *= -COLLISION_DAMP;
            }
            // Top wall
            if (p.position.y < PARTICLE_RADIUS) {
                p.position.y = PARTICLE_RADIUS;
                p.velocity.y *= -COLLISION_DAMP;
            }
            // Bottom wall
            if (p.position.y > HEIGHT - PARTICLE_RADIUS) {
                p.position.y = HEIGHT - PARTICLE_RADIUS;
                p.velocity.y *= -COLLISION_DAMP;
            }
        }

        sf::Event event;

        while(window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mouseTarget(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                for (auto& p : particles) {
                    sf::Vector2f dir = mouseTarget - p.position;
                    float dist = length(dir);

                    if (dist < 150.0f && dist > 0.001f) {
                        sf::Vector2f  pullForce = (dir / dist) * 200.0f;
                        p.velocity += pullForce * dt;
                    }
                }
            }
        }

        window.clear(sf::Color(203, 195, 227)); // purple background just for fun i like it

        for (const auto& p : particles) {

            float speed = length(p.velocity);
            float t = std::min(speed / MAX_SPEED, 1.0f);

            sf::Uint8 r, g, b;
            r = static_cast<sf::Uint8>(t * 255.0f);
            g = 0;
            b = static_cast<sf::Uint8>((1.0f - t) * 255.0f);

            render_shape.setFillColor(sf::Color(r, g, b));
            render_shape.setPosition(p.position.x, p.position.y);
            
            window.draw(render_shape);
        }

        window.display();
    }

    return EXIT_SUCCESS;
}