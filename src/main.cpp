#include <libdragon.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/gl_integration.h>
#include "camera.hpp"
#include "bulletClass.hpp"
#include "main.hpp"

extern "C" {
    #include "../lib/microuiN64.h"
}

#define TARGET_FRAME_TIME_MS 33.33                                         // milliseconds for 30 FPS
#define TARGET_FRAME_TIME (TARGET_FRAME_TIME_MS * TICKS_PER_SECOND / 1000) // converting milliseconds to ticks - TICKS_PER_SECOND is defined in libdragon

// Set up the Scene
float sceneRotation = 0.0f;
surface_t zbuffer;
camera_t camera = {100, 0};

static const int kPrismCount = 0;

// Allocate the prism display list
static GLuint prismDisplayList;

// Enstantiate the physics object
PhysicsObjectClass enstantiatedPhysicsObject;

// Initialize plane rotation
float plane_rotationX = 0.0f;
float plane_rotationZ = 0.0f;

// Variables for handling FPS
double frame_start;
double frame_end;
double frame_duration;

static int debugDraw = 1;

void DrawSimulationSettingsWindow()
{
    mu64_set_mouse_speed(0.000000001f * frame_duration);

    if (mu_begin_window_ex(&mu_ctx, "Simulation Config", mu_rect(12, 20, 90, 140), MU_OPT_NOCLOSE))
    {
        mu_checkbox(&mu_ctx, "DebugDraw", &debugDraw);

        if (mu_button(&mu_ctx, "Ragdoll"))
        {
            enstantiatedPhysicsObject.createRagdoll();
        }

        if (mu_button(&mu_ctx, "Prism"))
        {
            enstantiatedPhysicsObject.createPrismRigidBody();
        }

        mu_end_window(&mu_ctx);
    }
}

void render()
{
    // Framerate handling
    frame_start = get_ticks();

    surface_t *disp = display_get();
    rdpq_attach(disp, &zbuffer);

    gl_context_begin();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    camera_transform(&camera);
    glRotatef(90, 0.0f, 1.0f, 0.0f);

    // Rotate the scene
    // sceneRotation += 0.2f;
    glRotatef(sceneRotation, 0.0f, 1.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);

    // Step the simulation - varies based on FPS
    enstantiatedPhysicsObject.stepSimulation();

#if USE_PHYSICS_DEBUG_DRAW
    enstantiatedPhysicsObject.DebugDrawWorld(debugDraw);
#endif

    drawPlane();

    drawPrismFromDisplayList(&enstantiatedPhysicsObject);

    gl_context_end();

    mu64_draw(); // Step 5/5: render out the UI at the very end

    rdpq_detach_wait(); // Wait for the RDP queue to finish otherwise the display won't properly handle the text

    // Handle FPS
    drawFPS(disp);
    frame_end = get_ticks();
    frame_duration = frame_end - frame_start;
    double frame_duration_ms = (double)frame_duration / (TICKS_PER_SECOND / 1000);
    printf("Frame start (milliSec): %f\n", frame_start / (TICKS_PER_SECOND / 1000));
    printf("Frame end (milliSec): %f\n", frame_end / (TICKS_PER_SECOND / 1000));
    printf("Frame duration (milliSec): %f\n", frame_duration_ms);
    if (frame_duration_ms < TARGET_FRAME_TIME_MS)

    {
        printf("Sleeping for %f\n", TARGET_FRAME_TIME_MS - frame_duration_ms);
        wait_ms(TARGET_FRAME_TIME_MS - frame_duration_ms);
    }

    // Message on screen for controls
    graphics_draw_text(disp, 15, 15, "Controls:");
    graphics_draw_text(disp, 15, 25, "Analog Stick: Move Plane");
    graphics_draw_text(disp, 15, 35, "A: Reset Prisms");

    // Don't forget to update the display
    display_show(disp);
}

void setup()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)display_get_width() / (float)display_get_height(), 50.0f, 400.0f);
    zbuffer = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height());

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    camera.distance = -100;
    camera.rotationY = -22.5;

    // Set up the prism display list - this is a static object so we only need to do this once even if we draw it multiple times
    setupPrismDisplayList();

    // Create the physics object
    enstantiatedPhysicsObject.initializePhysics();
    for (int i = 0; i < kPrismCount; i++)
    {
        enstantiatedPhysicsObject.createPrismRigidBody();
    }
    enstantiatedPhysicsObject.createGroundRigidBody();
}

void drawPlane() // The plane will act as the ground
{
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);

    // Rotate the plane if we want to
    glRotatef(plane_rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(plane_rotationZ, 0.0f, 0.0f, 1.0f);

    // Update the physics object with the plane's rotation
    enstantiatedPhysicsObject.updatePlaneRotation(plane_rotationX, plane_rotationZ);

    glBegin(GL_QUADS);
    // Making each vertex a different color for demonstration purposes
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-100.0f, 0.0f, -100.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 100.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 100.0f);
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, -100.0f);
    glEnd();
    glPopMatrix();
}

void setupPrismDisplayList()
{
    // Allocate the prism display list
    prismDisplayList = glGenLists(1);

    // Create the prism display list
    glNewList(prismDisplayList, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    // Front
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 9.0f, 0.0f);
    glVertex3f(-3.0f, 0.0f, 3.0f);
    glVertex3f(3.0f, 0.0f, 3.0f);
    // Right
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 9.0f, 0.0f);
    glVertex3f(3.0f, 0.0f, 3.0f);
    glVertex3f(3.0f, 0.0f, -3.0f);
    // Back
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 9.0f, 0.0f);
    glVertex3f(3.0f, 0.0f, -3.0f);
    glVertex3f(-3.0f, 0.0f, -3.0f);
    // Left
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 9.0f, 0.0f);
    glVertex3f(-3.0f, 0.0f, -3.0f);
    glVertex3f(-3.0f, 0.0f, 3.0f);
    glEnd();
    glEndList();
}

void drawPrismFromDisplayList(PhysicsObjectClass *localPhysicsObject)
{
    for (auto *prismRigidBody : localPhysicsObject->getPrismRigidBodies())
    {
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);

        glEnable(GL_DEPTH_TEST);

        // Get position and rotation of the current prism
        btVector3 position = localPhysicsObject->getPrismRigidBodyPosition(prismRigidBody);
        btQuaternion rotation = localPhysicsObject->getPrismRigidBodyRotation(prismRigidBody);

        // Apply the physics transform to the prism
        glTranslatef(position.getX(), position.getY(), position.getZ());

        // Convert quaternion to angle-axis representation for OpenGL rotation
        btScalar angle = rotation.getAngle();
        btVector3 axis = rotation.getAxis();
        glRotatef(angle * 180 / SIMD_PI, axis.getX(), axis.getY(), axis.getZ()); // Convert radians to degrees

        glCallList(prismDisplayList);
        glPopMatrix();
    }
}

int main()
{
    // Initialize debug console
    debug_init_isviewer();
    console_init();
    console_set_render_mode(RENDER_MANUAL);
    console_set_debug(1);

    joypad_init();

    dfs_init(DFS_DEFAULT_LOCATION);

    // Setting up the display and GL
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS_DEDITHER);
    rdpq_init();
    gl_init();

    // Step 1/5: Make sure you have a small font loaded
    rdpq_font_t *font = rdpq_font_load("rom:/VCR_OSD_MONO.font64");
    uint8_t font_id = 1;
    rdpq_text_register_font(font_id, font);

    // Step 2/5: init UI libary, pass in the controller (joystick or N64-mouse) and font-id
    // (Note: take a look inside this function for styling and controls.)
    mu64_init(JOYPAD_PORT_1, font_id);

    // Setting up the camera
    setup();

    // continuously loop through the render function
    while (1)
    {
        joypad_poll();

        mu64_start_frame();// Step 3/5: call this BEFORE your game logic starts each frame
        DrawSimulationSettingsWindow();
        handleControls();
        mu64_end_frame(); // Step 4/5: call this AFTER your game logic ends each frame

        render();
    }

    printf("Intro finished\n");
    gl_close();
    rdpq_close();
    display_close();
}

// Controls to move the plane around
void handleControls()
{
    int vertical_input = joypad_get_axis_held(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_Y);
    int horizontal_input = joypad_get_axis_held(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_X);

    plane_rotationX += horizontal_input * 0.3;
    plane_rotationZ -= vertical_input * 0.3;

/*
    joypad_buttons_t button_pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if (button_pressed.a)
    {
        enstantiatedPhysicsObject.resetPrismRigidBodies();
    }
*/
}

void drawFPS(surface_t *localDisplay)
{
    // Get the current FPS
    float fps = display_get_fps();

    // Print the FPS to the screen
    graphics_draw_text(localDisplay, 15, 5, std::to_string(fps).c_str());
}