
#include "modeling.hpp"


#ifdef SCENE_3D_GRAPHICS

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;




float evaluate_terrain_z(float u, float v);
vec3 evaluate_terrain(float u, float v);
mesh create_terrain();
mesh create_cylinder(float radius, float height);
mesh create_cone(float radius, float height, float z_offset);
mesh create_tree_foliage(float radius, float height, float z_offset);
mesh create_billboard();
mesh create_strange_billboard();
mesh create_sky(float dimension);
std::vector<vcl::vec3> update_tree_position();
std::vector<vcl::vec3> update_champignon_position();
std::vector<vcl::vec3> update_grass_position();
std::vector<vcl::vec3> update_flower_position();


/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& )
{
    // Create visual terrain surface
    terrain = create_terrain();
    terrain.uniform.color = {0.6f,0.85f,0.5f};
    terrain.uniform.shading.specular = 0.0f; // non-specular terrain material

    // Cylinder
    cylinder = create_cylinder(0.15f, 1.0f);
    cylinder.uniform.color = {0.282f, 0.117f, 0.117f};
    cylinder.uniform.shading.specular = 0.0f; // non-specular terrain material

    // Cone
    cone = create_tree_foliage(0.5f, 0.8f, 0.3f);
    cone.uniform.color = {0.0f, 1.0f, 0.0f};
    cone.uniform.shading.specular = 0.0f; // non-specular terrain material

    // Trees' position
    tree_position = update_tree_position();

    // Champignon
    // Cylinder
    champ_cylinder = create_cylinder(0.05f, 0.15f);
    champ_cylinder.uniform.color = {0.705f, 0.611f, 0.611f};
    champ_cylinder.uniform.shading.specular = 0.0f;
    // Cone
    champ_cone = create_cone(0.15f, 0.1f, 0.15f);
    champ_cone.uniform.color = {0.866f, 0.113f, 0.113f};
    champ_cone.uniform.shading.specular = 0.0f;

    // Champignons' position
    champignon_position = update_champignon_position();

    // Sky
    sky = create_sky(40.0f);
    sky.uniform.shading = {1,0,0};

    // Grass
    grass = create_billboard();
    grass.uniform.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
    strange_grass = create_strange_billboard();
    strange_grass.uniform.shading = {1,0,0};

    // Grass position
    grass_position = update_grass_position();

    // Flower
    flower = create_billboard();
    flower.uniform.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
    strange_flower = create_strange_billboard();
    strange_flower.uniform.shading = {1,0,0};

    // Flower position
    flower_position = update_flower_position();

    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 10.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);
    mat3 M = scene.camera.orientation;
    std::cout << M << std::endl;

    // Textures
    // Terrain
    texture_terrain_id = create_texture_gpu( image_load_png("scenes/3D_graphics/02_texture/assets/grass.png")); // Load a texture image on GPU and stores its ID
    terrain.texture_id = texture_terrain_id;

    // Grass
    // Load a texture (with transparent background)
    texture_grass_id = create_texture_gpu( image_load_png("scenes/3D_graphics/02_texture/assets/billboard_grass.png"), GL_REPEAT, GL_REPEAT );

    // Flower
    // Load a texture (with transparent background)
    texture_flower_id = create_texture_gpu( image_load_png("scenes/3D_graphics/02_texture/assets/pinkflower.png"), GL_REPEAT, GL_REPEAT );

    // Sky
    texture_sky_id = create_texture_gpu( image_load_png("scenes/3D_graphics/02_texture/assets/universe4.png"));
}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    set_gui();

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe


    // ***** DISPLAY TERRAIN ***** //
    //
    glPolygonOffset( 1.0, 1.0 );

    // Before displaying a textured surface: bind the associated texture id
    glBindTexture(GL_TEXTURE_2D, texture_terrain_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    draw(terrain, scene.camera, shaders["mesh"]);
    // After the surface is displayed it is safe to set the texture id to a white image
    // Avoids to use the previous texture for another object
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        draw(terrain, scene.camera, shaders["wireframe"]);
    }

    // ***** DISPLAY TREES ***** //
    //
    for(std::vector<vcl::vec3>::iterator it = tree_position.begin(); it < tree_position.end(); it++){
        // Cylinder
        cylinder.uniform.transform.translation = *it;
        draw(cylinder, scene.camera, shaders["mesh"]);
        // Cone
        cone.uniform.transform.translation = *it + vec3(0.0f, 0.0f, 1.0f);
        draw(cone, scene.camera, shaders["mesh"]);
        if( gui_scene.wireframe ){ // wireframe if asked from the GUI
            draw(cylinder, scene.camera, shaders["wireframe"]);
            draw(cone, scene.camera, shaders["wireframe"]);
        }
    }

    // ***** DISPLAY CHAMPIGNONS ***** //
    //
    for(std::vector<vcl::vec3>::iterator it = champignon_position.begin(); it < champignon_position.end(); it++){
        // Cylinder
        champ_cylinder.uniform.transform.translation = *it;
        draw(champ_cylinder, scene.camera, shaders["mesh"]);
        // Cone
        champ_cone.uniform.transform.translation = *it;
        draw(champ_cone, scene.camera, shaders["mesh"]);
        if( gui_scene.wireframe ){ // wireframe if asked from the GUI
            draw(champ_cylinder, scene.camera, shaders["wireframe"]);
            draw(champ_cone, scene.camera, shaders["wireframe"]);
        }
    }

    // ***** DISPLAY SKY ***** //
    // Before displaying a textured surface: bind the associated texture id
    glBindTexture(GL_TEXTURE_2D, texture_sky_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    //sky.uniform.transform.translation = scene.camera.camera_position();
    draw(sky, scene.camera, shaders["mesh"]);
    // After the surface is displayed it is safe to set the texture id to a white image
    // Avoids to use the previous texture for another object
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    if(gui_scene.wireframe)
        draw(sky, scene.camera, shaders["wireframe"]);


    // ***** DISPLAY GRASS ***** //
    //
    // Enable use of alpha component as color blending for transparent elements
    //  new color = previous color + (1-alpha) current color
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth buffer writing
    //  - Transparent elements cannot use depth buffer
    //  - They are supposed to be display from furest to nearest elements
    glDepthMask(false);


    glBindTexture(GL_TEXTURE_2D, texture_grass_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts

    for(std::vector<vcl::vec3>::iterator it = grass_position.begin(); it < grass_position.end(); it++){
        // Display a billboard always facing the camera direction
        // ********************************************************** //
        strange_grass.uniform.transform.rotation = scene.camera.orientation;
        strange_grass.uniform.transform.translation = *it;
        draw(strange_grass, scene.camera, shaders["mesh"]);
        if(gui_scene.wireframe)
            draw(strange_grass, scene.camera, shaders["wireframe"]);

        // Display two orthogonal billboards with static orientation
        // ********************************************************** //
        const mat3 Identity = mat3::identity();
        const mat3 R = rotation_from_axis_angle_mat3({0,0,1}, 3.14f/2.0f); // orthogonal rotation

        grass.uniform.transform.translation = *it;
        grass.uniform.transform.rotation = Identity;
        draw(grass, scene.camera, shaders["mesh"]);
        if(gui_scene.wireframe)
            draw(grass, scene.camera, shaders["wireframe"]);

        grass.uniform.transform.rotation = R;
        draw(grass, scene.camera, shaders["mesh"]);
        if(gui_scene.wireframe)
            draw(grass, scene.camera, shaders["wireframe"]);
    }

    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glDepthMask(true);

    // ***** DISPLAY FLOWERS ***** //
    //
    // Enable use of alpha component as color blending for transparent elements
    //  new color = previous color + (1-alpha) current color
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth buffer writing
    //  - Transparent elements cannot use depth buffer
    //  - They are supposed to be display from furest to nearest elements
//    glDepthMask(false);


//    glBindTexture(GL_TEXTURE_2D, texture_flower_id);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts

//    for(std::vector<vcl::vec3>::iterator it = flower_position.begin(); it < flower_position.end(); it++){
//        // Display a billboard always facing the camera direction
//        // ********************************************************** //
//        strange_flower.uniform.transform.rotation = scene.camera.orientation;
//        strange_flower.uniform.transform.translation = *it;
//        //draw(strange_flower, scene.camera, shaders["mesh"]);
//        //if(gui_scene.wireframe)
//            //draw(strange_flower, scene.camera, shaders["wireframe"]);

//        // Display two orthogonal billboards with static orientation
//        // ********************************************************** //
//        const mat3 Identity = mat3::identity();
//        const mat3 R = rotation_from_axis_angle_mat3({0,0,1}, 3.14f/2.0f); // orthogonal rotation

//        strange_flower.uniform.transform.rotation = Identity;
//        //draw(grass, scene.camera, shaders["mesh"]);
//        //if(gui_scene.wireframe)
//            //draw(strange_flower, scene.camera, shaders["wireframe"]);

//        strange_flower.uniform.transform.rotation = R;
//        //draw(grass, scene.camera, shaders["mesh"]);
//        //if(gui_scene.wireframe)
//            //draw(strange_flower, scene.camera, shaders["wireframe"]);
//    }

//    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
//    glDepthMask(true);

}



// Evaluate height of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_z(float u, float v)
{
    //const vec2 u0 = {0.5f, 0.5f};
    const buffer<vec2> ui = {{0.0f, 0.0f}, {0.5f, 0.5f}, {0.2f, 0.7f}, {0.8f, 0.7f}};

    //const float h0 = 2.0f;
    const buffer<float> hi = {3.0f, -1.5f, 1.0f, 2.0f};

    //const float sigma0 = 0.15f;
    const buffer<float> sigmai = {0.5f, 0.15f,0.2f, 0.2f};

    float sum = 0;
    float d = 0;

    //const float d = norm(vec2(u,v)-u0)/sigma0;
    for(int i = 0; i < 4; i++){
        d = norm(vec2(u,v)-ui[i])/sigmai[i];
        sum += hi[i]*std::exp(-d*d);
    }

    const float z = sum;

    return z;
}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v)
{
    const float x = 20*(u-0.5f);
    const float y = 20*(v-0.5f);

    // Perlin parameters
    const float scaling = 5.0f;
    const int octave = 7;
    const float persistency = 0.6f;
    const float height = 0.3f;

    // Evaluate Perlin noise
    const float noise = perlin(scaling*u, scaling*v, octave, persistency);

    const float z = evaluate_terrain_z(u,v) + height*noise;

    return {x,y,z};
}

// Generate terrain mesh
mesh create_terrain()
{
    // Number of samples of the terrain is N x N
    const size_t N = 100;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);
    terrain.texture_uv.resize(N*N);

    // Fill terrain geometry
    for(size_t ku=0; ku<N; ++ku)
    {
        for(size_t kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            // Compute coordinates
            terrain.position[kv+N*ku] = evaluate_terrain(u,v);
            terrain.texture_uv[kv+N*ku] = {kv/float(10),1 - ku/float(10)};
        }
    }


    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    const unsigned int Ns = N;
    for(unsigned int ku=0; ku<Ns-1; ++ku)
    {
        for(unsigned int kv=0; kv<Ns-1; ++kv)
        {
            const unsigned int idx = kv + N*ku; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1+Ns, idx+1};
            const uint3 triangle_2 = {idx, idx+Ns, idx+1+Ns};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    return terrain;
}


// Generate cylinder mesh
mesh create_cylinder(float radius, float height){
    // Number of samples of the cylinder is 2 x N
    const int N = 20;

    mesh cylinder; // temporary cylinder storage (CPU only)

    // Fill cylinder geometry
    for(size_t i=0; i<N; i++){
        float u = i/float(N);
        cylinder.position.push_back({radius*std::cos(2*3.14f*u), radius*std::sin(2*3.14f*u), 0.0f});
        cylinder.position.push_back({radius*std::cos(2*3.14f*u), radius*std::sin(2*3.14f*u), height});
    }

    // Generate triangle organization
    // Each 3 consecutive points form a triangle
    uint3 triangle;
    for(uint i=0; i<2*N; i++){
        triangle = {i, (i+1)%(2*N), (i+2)%(2*N)};
        cylinder.connectivity.push_back(triangle);
    }

    return cylinder;
}


// Generate cone mesh
mesh create_cone(float radius, float height, float z_offset){

    // Number of samples of the cone is N+2
    const size_t N = 20;

    mesh cone; // temporary cone storage (CPU only)

    // Fill cone geometry
    // Base points in positions 0 to N-1
    for(size_t i=0; i<N; i++){
        float u = i/float(N);
        cone.position.push_back({radius*std::cos(2*3.14f*u), radius*std::sin(2*3.14f*u), z_offset});
    }
    // Center of the base in position N
    cone.position.push_back({0.0f, 0.0f, z_offset});
    // Top of the cone in position N+1
    cone.position.push_back({0.0f, 0.0f, z_offset+height});

    // Connectivity
    uint3 triangle;
    for(uint i=0; i<N; i++){
        triangle = {i, (i+1)%N, N};
        cone.connectivity.push_back(triangle);
        triangle = {i, (i+1)%N, N+1};
        cone.connectivity.push_back(triangle);
    }

    return cone;
}

// Generate feuillage
mesh create_tree_foliage(float radius, float height, float z_offset)
{
    mesh m = create_cone(radius, height, 0);
    m.push_back( create_cone(radius, height, z_offset) );
    m.push_back( create_cone(radius, height, 2*z_offset) );

    return m;
}

mesh create_billboard(){
    mesh billboard;
    billboard.position     = {{-0.2f,0,0}, { 0.2f,0,0}, { 0.2f,0,0.4f}, {-0.2f,0,0.4f}};
    billboard.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
    billboard.connectivity = {{0,1,2}, {0,2,3}};
    return billboard;
}

mesh create_strange_billboard(){
    mesh billboard;
    billboard.position     = {{-0.2f,0,0}, { 0.2f,0,0}, { 0.2f, 0.4f,0}, {-0.2f, 0.4f,0}};
    billboard.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
    billboard.connectivity = {{0,1,2}, {0,2,3}};
    return billboard;
}

mesh create_sky(float dimension){
    mesh sky;
    // Geometry
    float l = dimension/2.0f;
    mesh base;
    mesh face1;
    mesh face2;
    mesh face3;
    mesh face4;
    mesh top;

    base.position = {{-l,-l,-l}, {l,-l,-l}, {l,l,-l}, {-l,l,-l}};
    base.texture_uv = {{1/4.0f, 2/3.0f}, {2/4.0f, 2/3.0f}, {2/4.0f, 1.0f}, {1/4.0f, 1.0f}};
    base.connectivity = {{0,1,2}, {0,2,3}};

    face1.position = {{-l,-l,-l}, {l,-l,-l}, {-l,-l,l}, {l,-l,l}};
    face1.texture_uv = {{1/4.0f, 2/3.0f}, {2/4.0f, 2/3.0f}, {1/4.0f, 1/3.0f}, {2/4.0f, 1/3.0f}};
    face1.connectivity = {{0,1,2}, {1,2,3}};

    face2.position = {{l,-l,-l}, {l,l,-l}, {l,-l,l}, {l,l,l}};
    face2.texture_uv = {{2/4.0f, 2/3.0f}, {3/4.0f, 2/3.0f}, {2/4.0f, 1/3.0f}, {3/4.0f, 1/3.0f}};
    face2.connectivity = {{0,1,2}, {1,2,3}};

    face3.position = {{l,l,-l}, {-l,l,-l}, {l,l,l}, {-l,l,l}};
    face3.texture_uv = {{3/4.0f, 2/3.0f}, {1.0f, 2/3.0f}, {3/4.0f, 1/3.0f}, {1.0f, 1/3.0f}};
    face3.connectivity = {{0,1,2}, {1,2,3}};

    face4.position = {{-l,l,-l}, {-l,-l,-l}, {-l,-l,l}, {-l,l,l}};
    face4.texture_uv = {{0, 2/3.0f}, {1/4.0f, 2/3.0f}, {1/4.0f, 1/3.0f}, {0, 1/3.0f}};
    face4.connectivity = {{0,1,2}, {0,2,3}};

    top.position = {{-l,-l,l}, {l,-l,l}, {l,l,l}, {-l,l,l}};
    top.texture_uv = {{1/4.0f, 1/3.0f}, {2/4.0f, 1/3.0f}, {2/4.0f, 0}, {1/4.0f, 0}};
    top.connectivity = {{0,1,2}, {0,2,3}};

    sky.push_back(base);
    sky.push_back(face1);
    sky.push_back(face2);
    sky.push_back(face3);
    sky.push_back(face4);
    sky.push_back(top);
    return sky;
}

// Generate trees' position
std::vector<vcl::vec3> update_tree_position(){

    std::vector<vcl::vec3> positions;

    // Nombre d'arbres
    const size_t division = 6; // To generate division*division areas, each one with a tree inside
    const float margin = 1/(division*10.0f);  // To avoid tree intersection
    //const size_t Na = division*division;
    for(int i = 0; i<division; i++){
        for(int j=0; j<division; j++){
            float au = i/float(division);
            float bu = (i+1)/float(division);
            float av = j/float(division);
            float bv = (j+1)/float(division);
            float u = rand_interval(au+margin,bu-margin);
            float v = rand_interval(av+margin,bv-margin);
            vec3 p = evaluate_terrain(u,v);
            p += {0.0f, 0.0f, -0.1f};
            positions.push_back(p);
        }
    }

    return positions;
}

// Generate champignons' position
std::vector<vcl::vec3> update_champignon_position(){

    std::vector<vcl::vec3> positions;

    // Nombre de champignons
    const size_t division = 5; // To generate division*division areas, each one with a champignon inside
    const float margin = 1/(division*10.0f);  // To avoid champignon intersection
    //const size_t Na = division*division;
    for(int i = 0; i<division; i++){
        for(int j=0; j<division; j++){
            float au = i/float(division);
            float bu = (i+1)/float(division);
            float av = j/float(division);
            float bv = (j+1)/float(division);
            float u = rand_interval(au+margin,bu-margin);
            float v = rand_interval(av+margin,bv-margin);
            vec3 p = evaluate_terrain(u,v);
            p += {0.0f, 0.0f, -0.01f};
            positions.push_back(p);
        }
    }

    return positions;
}

// Generate trees' position
std::vector<vcl::vec3> update_grass_position(){

    std::vector<vcl::vec3> positions;

    // Nombre d'arbres
    const size_t division = 6; // To generate division*division areas, each one with a tree inside
    const float margin = 1/(division*10.0f);  // To avoid tree intersection
    //const size_t Na = division*division;
    for(int i = 0; i<division; i++){
        for(int j=0; j<division; j++){
            float au = i/float(division);
            float bu = (i+1)/float(division);
            float av = j/float(division);
            float bv = (j+1)/float(division);
            float u = rand_interval(au+margin,bu-margin);
            float v = rand_interval(av+margin,bv-margin);
            vec3 p = evaluate_terrain(u,v);
            //p += {0.0f, 0.0f, -0.1f};
            positions.push_back(p);
        }
    }

    return positions;
}

// Generate trees' position
std::vector<vcl::vec3> update_flower_position(){

    std::vector<vcl::vec3> positions;

    // Nombre d'arbres
    const size_t division = 4; // To generate division*division areas, each one with a tree inside
    const float margin = 1/(division*10.0f);  // To avoid tree intersection
    //const size_t Na = division*division;
    for(int i = 0; i<division; i++){
        for(int j=0; j<division; j++){
            float au = i/float(division);
            float bu = (i+1)/float(division);
            float av = j/float(division);
            float bv = (j+1)/float(division);
            float u = rand_interval(au+margin,bu-margin);
            float v = rand_interval(av+margin,bv-margin);
            vec3 p = evaluate_terrain(u,v);
            //p += {0.0f, 0.0f, -0.1f};
            positions.push_back(p);
        }
    }

    return positions;
}

void scene_model::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
}


#endif

