#include "SFML/Graphics.hpp"
#include <vector>

/*
Simple gravity simulator
*/

class GravitySource {

    sf::Vector2f pos; //Position of the source
    float strength; //Pulling strength
    sf::CircleShape s; //Shape

public:
    GravitySource(float pos_x, float pos_y, float strength) { //Constructor
        pos.x = pos_x;
        pos.y = pos_y;
        this->strength = strength;

        s.setPosition(pos);
        s.setFillColor(sf::Color::White);
        s.setRadius(10);
    }

    void render(sf::RenderWindow& wind) { //Function to render the shape
        wind.draw(s);
    }

    sf::Vector2f get_pos() { //Getter for the pos
        return pos;
    }

    float get_strength() { //Getter for the strength
        return strength;
    }

};


class Particle {

    sf::Vector2f pos;
    sf::Vector2f vel;//The velocity of the particle
    sf::CircleShape s;

public:
    Particle(float pos_x, float pos_y, float vel_x, float vel_y) {
        pos.x = pos_x;
        pos.y = pos_y;

        vel.x = vel_x;
        vel.y = vel_y;

        s.setPosition(pos);
        s.setFillColor(sf::Color::White);
        s.setRadius(8);
    }

    void render(sf::RenderWindow& wind) {
        s.setPosition(pos);
        wind.draw(s);
    }

    void update_physics(GravitySource& s) { //Function that updates the physics of the particle

        float distance_x = s.get_pos().x - pos.x;//The distance between a particle and a source on the x axis
        float distance_y = s.get_pos().y - pos.y;//The distance between a particle and a source on the y axis

        float distance = sqrt(distance_x * distance_x + distance_y * distance_y);//Using Pythagora's theorem calculate the direct distance between a particle and a source

        float inverse_distance = 1.f / distance; //We calculate inverse_distance to normalize x and y

        float normalized_x = inverse_distance * distance_x;
        float normalized_y = inverse_distance * distance_y;

        float inverse_square_dropoff = inverse_distance * inverse_distance;//To simulate a weaker pulling force when the particle is farther away we calculate inverse_square_dropoff.

        float acceleration_x = normalized_x * s.get_strength() * inverse_square_dropoff;//We calculate the acceleration
        float acceleration_y = normalized_y * s.get_strength() * inverse_square_dropoff;

        vel.x += acceleration_x;//Update the velocity
        vel.y += acceleration_y;

        pos.x += vel.x;//Update the position
        pos.y += vel.y;
    }

    void set_color(sf::Color col) {//Function to set the color of a particle
        s.setFillColor(col);
    }

};


sf::Color map_val_to_color(float value) {//Function that makes a linear transition from blue to green and from green to red
    if (value < 0.0f) value = 0;
    if (value > 1.0f) value = 1;

    int r = 0, b = 0, g = 0;

    //0-0.5 interpolate blue to green
    //0.5-1 interpolate green to red

    if (value < 0.5f) {
        b = 255 * (1.0f - 2 * value);
        g = 255 * 2 * value;
    }
    else {
        g = 255 * (2.0f - 2 * value);
        r = 255 * (2 * value - 1);
    }

    return sf::Color(r, g, b);
}



int main()
{
    sf::RenderWindow window(sf::VideoMode(1600, 1000), "Test");//Creating the window
    window.setFramerateLimit(60);  //Framerate limit

    std::vector<GravitySource> sources;
    sources.push_back(GravitySource(500, 500, 7000));
    sources.push_back(GravitySource(1200, 500, 7000));

    int num_particles = 2000;

    std::vector<Particle> particles;

    for (int i = 0; i < num_particles; i++) {
        particles.push_back(Particle(600, 700, 4, 0.2 + (0.1 / num_particles) * i));//Generating multiple particles with different y velocity for different behaviour

        //Change colors
        float val = (float)i / (float)num_particles;//Spread equaly the colors for each particle
        
        sf::Color col = map_val_to_color(val);

        particles[i].set_color(col);

    }

    while (window.isOpen()) {
        sf::Event e; 
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
                
        }

        window.clear();
        
        for (int i = 0; i < sources.size() ; i++) {//Applying the pulling force of each source to each particle
            for (int j = 0; j < particles.size(); j++) {
                particles[j].update_physics(sources[i]);
            }
        }

        for (int i = 0; i < sources.size(); i++) {//Render each source
            sources[i].render(window);
        }

        for (int i = 0; i < particles.size(); i++) {//Render each particle
            particles[i].render(window);
        }

        window.display();
    }



    return 0;
}
