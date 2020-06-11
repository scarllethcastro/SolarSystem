#pragma once

#include "main/scene_base/base.hpp"


// Store a vec3 (p) + time (t)
struct vec3t{
    vcl::vec3 p; // position
    float t;     // time
};

// Stores some parameters that can be set from the GUI
struct gui_scene_structure
{
    bool display_keyframe = true;
    bool display_polygon  = true;
    bool wireframe   = false;
    bool surface     = true;
    bool skeleton    = false;
    bool stars[10] = {false, false, false, false, false, false, false, false, false, false};

};

struct star {
    float radius;
    float mass;
    GLuint texture;
    vcl::mesh_drawable drawable;
    vcl::vec3 p ;
    vcl::vec3 v ;
};

struct planet: star {
    float inclination;
    vcl::vec3 force;
    float orbit_radius; // axis a
    float vel_rot;
    float hour =0;

};

struct scene_model : scene_base
{
    /** A part must define two functions that are called from the main function:
     * setup_data: called once to setup data before starting the animation loop
     * frame_draw: called at every displayed frame within the animation loop
     *
     * These two functions receive the following parameters
     * - shaders: A set of shaders.
     * - scene: Contains general common object to define the 3D scene. Contains in particular the camera.
     * - data: The part-specific data structure defined previously
     * - gui: The GUI structure allowing to create/display buttons to interact with the scene.
    */


    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);

    void set_gui(vcl::timer_basic& timer);
    void camera_position_at_each_star(scene_structure& scene);

    // Setup functions
    void setup_universe();
    void setup_sun();
    void setup_mercury();
    void setup_venus();
    void setup_earth();
    void setup_mars();
    void setup_jupiter();
    void setup_saturn();
    void setup_uranus();
    void setup_neptune();
    void setup_moon();

    // Draw functions
    void draw_universe(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void draw_sun(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void draw_planets(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void draw_moon(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void draw_saturn_ring(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void draw_sun_ring(std::map<std::string,GLuint>& shaders, scene_structure& scene);

    // Update data functions
    void update_position_planets(float dt);
    void update_position_moon(float dt);
    void update_position_saturn_ring();

    // visual representation of a surface
    gui_scene_structure gui_scene;


    vcl::timer_interval timer;    // Timer allowing to indicate periodic events

    // Universe
    vcl::mesh_drawable universe;

    // Sun
    star sun;
    vcl::mesh_drawable sun_ring;

    // Planets
    std::vector<planet> planets;

    // Saturn ring
    planet saturn_ring;

    // Moon
    planet moon;

    // Textures
    GLuint texture_universe_id;
    GLuint texture_sun_id;
};



