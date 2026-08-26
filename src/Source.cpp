/* * Project: GALAGA X
 * Major: Computer Science
 * Course: Computer Graphics (CS2206)
 * Group Members: Yara Alsulami, Daniah Hadi, Wasan Alwafi, Reyof Alahmari, Rafah Asharif, Riham Wan-Deraman
 * Date: May 2026
 */
#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <ctime>
#include <cmath>
#include <string>

 // Glopal variables for screen and player settings
int width = 600, height = 800;
float playerX = 305.0f, playerY = 100.0f; // Initial player position
float playerSpeed = 18.0f;
// Structs to represent game objects
struct WoodBox { float x, y, size, speed; };
struct Bomb { float x, y, size, speed; };
std::vector<Bomb> bombs;
std::vector<WoodBox> boxes;

// Game state variables
int hearts = 3, score = 0;
bool gameOver = false, gameWin = false;
int scene = 0;      // 0: Menu, 1: Playing screen
int gameLevel = 1;  // 1, 2, 3
int difficulty = 0; // 0: Easy, 1: Medium, 2: Hard

// Level balancing settings
float currentSpeed = 4.0f;
int spawnRate = 80;

// Animation and status variables
bool isInvincible = false;
int invincibilityTimer = 0;
float groundMove = 0.0f, skyMove = 0.0f;

// Texture identifiers & path
GLuint groundTex, skyTex;
const char* groundTexPath = "Assets/ground.bmp";
const char* skyTexPath = "Assets/sky.bmp";


//---------------------------------------------------------
// UI Drawing Functions 
//---------------------------------------------------------

// Draws a solid filled rectangle
void drawRect(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

// Draws the outline of a rectangle
void drawRectBorder(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}
// Renders text on the screen at specific coordinates
void drawTextUI(float x, float y, std::string text, void* font) {
    glRasterPos2f(x, y);
    for (char c : text) glutBitmapCharacter(font, c);
}
// Draws interactive buttons for the menu
void drawButton(float x1, float y1, float x2, float y2,
    std::string label, float textX, float textY,
    float r, float g, float b, bool selected) {

    // Change brightness based on selection state
    glColor3f(selected ? r * 0.35f : r * 0.12f,
        selected ? g * 0.35f : g * 0.12f,
        selected ? b * 0.35f : b * 0.12f);
    drawRect(x1, y1, x2, y2);

    // Draw the button's border
    glColor3f(r, g, b);
    glLineWidth(2.0f);
    drawRectBorder(x1, y1, x2, y2);

    // Draw the button label
    glColor3f(r, g, b);
    drawTextUI(textX, textY, label, GLUT_BITMAP_HELVETICA_18);
}
// Draws a filled circle using a triangle fan
void drawCircleUI(float cx, float cy, float r, float R, float G, float B) {
    glColor3f(R, G, B);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 40; i++) {
        float angle = 2.0f * 3.14159f * i / 40;
        glVertex2f(cx + r * cos(angle), cy + r * sin(angle));
    }
    glEnd();
}
// Composes a cloud using three circles
void drawCloud(float x, float y, float scale) {
    drawCircleUI(x, y, 0.08f * scale, 1.0f, 1.0f, 1.0f);
    drawCircleUI(x - 0.07f * scale, y + 0.02f * scale, 0.06f * scale, 1.0f, 1.0f, 1.0f);
    drawCircleUI(x + 0.07f * scale, y + 0.02f * scale, 0.07f * scale, 1.0f, 1.0f, 1.0f);
}
// Draws stars
void drawStar(float cx, float cy, float r, float ir) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 10; i++) {
        // Radius alternates between outer (r) and inner (ir) points
        float radius = (i % 2 == 0) ? r : ir;
        float angle = -3.14159f / 2.0f + i * 3.14159f / 5.0f;
        glVertex2f(cx + radius * cos(angle), cy + radius * sin(angle));
    }
    glEnd();
}
// The main menu screen
void drawStartScreen() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    // Set camera to a fixed 2D range from -1.0 to 1.0
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Background gradient 
    glBegin(GL_QUADS);
    glColor3f(0.1f, 0.1f, 0.43f); glVertex2f(-1.0f, 1.0f);
    glColor3f(0.1f, 0.1f, 0.43f); glVertex2f(1.0f, 1.0f);
    glColor3f(0.53f, 0.81f, 0.92f); glVertex2f(1.0f, -1.0f);
    glColor3f(0.53f, 0.81f, 0.92f); glVertex2f(-1.0f, -1.0f);
    glEnd();

    // Draw fixed stars
    // Using array to store values
    float starX[] = { -0.85f, -0.60f, -0.30f, 0.10f, 0.45f, 0.70f, 0.88f };
    float starY[] = { 0.88f, 0.75f, 0.92f, 0.80f, 0.90f, 0.72f, 0.85f };
    for (int n = 0; n < 7; n++) {
        glPushMatrix();
        glTranslatef(starX[n], starY[n], 0.0f);
        glScalef(0.022f, 0.022f, 1.0f);
        glColor3f(1.0f, 1.0f, 0.85f);
        drawStar(0.0f, 0.0f, 1.0f, 0.4f);
        glPopMatrix();
    }

    // Draw fixed clouds
    drawCloud(-0.65f, 0.35f, 1.0f);
    drawCloud(0.60f, 0.42f, 0.85f);
    drawCloud(0.05f, 0.50f, 0.70f);

    // Title box decoration
    glColor3f(0.05f, 0.05f, 0.25f);
    drawRect(-0.70f, 0.28f, 0.70f, -0.05f);
    glColor3f(1.0f, 0.84f, 0.0f);
    glLineWidth(3.0f);
    drawRectBorder(-0.70f, 0.28f, 0.70f, -0.05f);
    // Title text
    glColor3f(1.0f, 0.84f, 0.0f);
    drawTextUI(-0.42f, 0.12f, "GALAGA X", GLUT_BITMAP_TIMES_ROMAN_24);
    // Controls info box
    glColor3f(0.05f, 0.05f, 0.25f);
    drawRect(-0.55f, -0.13f, 0.55f, -0.38f);
    glColor3f(0.27f, 0.67f, 1.0f);
    drawRectBorder(-0.55f, -0.13f, 0.55f, -0.38f);
    drawTextUI(-0.10f, -0.20f, "Controls", GLUT_BITMAP_HELVETICA_18);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawTextUI(-0.48f, -0.33f, " LEFT / RIGHT  ->  Move Plane ", GLUT_BITMAP_HELVETICA_18);
    // Difficulty selection text
    glColor3f(1.0f, 0.84f, 0.0f);
    drawTextUI(-0.18f, -0.59f, "Select Difficulty", GLUT_BITMAP_HELVETICA_18);
    // Difficulty buttons
    drawButton(-0.50f, -0.64f, -0.20f, -0.76f, "Easy", -0.45f, -0.72f, 0.4f, 1.0f, 0.4f, difficulty == 0);
    drawButton(-0.15f, -0.64f, 0.15f, -0.76f, "Medium", -0.13f, -0.72f, 1.0f, 1.0f, 0.4f, difficulty == 1);
    drawButton(0.20f, -0.64f, 0.50f, -0.76f, "Hard", 0.22f, -0.72f, 1.0f, 0.4f, 0.4f, difficulty == 2);
    // Start info
    glColor3f(1.0f, 1.0f, 1.0f);
    drawTextUI(-0.18f, -0.87f, "Press X to Play", GLUT_BITMAP_HELVETICA_18);
}
//---------------------------------------------------------
//Draw ellipse shape for engines and some parts
//---------------------------------------------------------
void drawEllipse(float cx, float cy, float rx, float ry, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(cx, cy, 0);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        glVertex3f(cx + cos(angle) * rx, cy + sin(angle) * ry, 0);
    }
    glEnd();
}
//------------------------------------------------------------------
// Draw heart shapes for life signs using drawEllipse method
//------------------------------------------------------------------
void drawHeart(float x, float y, float size) {
    glColor3f(1.0f, 0.2f, 0.3f);
    drawEllipse(x - size / 4, y, size / 4, size / 4, 20);
    drawEllipse(x + size / 4, y, size / 4, size / 4, 20);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - size / 2, y - size / 6);
    glVertex2f(x + size / 2, y - size / 6);
    glVertex2f(x, y - size);
    glEnd();
}
//---------------------------------------------------------
// Showing text on screen using GLUT fonts
//---------------------------------------------------------
void renderText(float x, float y, std::string text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}
//---------------------------------------------------------
// Draw wooden box 
//---------------------------------------------------------

void drawWoodBox(float x, float y, float size) {
    glColor3f(0.54f, 0.27f, 0.07f); // Brown color for wood
    glBegin(GL_QUADS);
    glVertex2f(x - size / 2, y - size / 2);
    glVertex2f(x + size / 2, y - size / 2);
    glVertex2f(x + size / 2, y + size / 2);
    glVertex2f(x - size / 2, y + size / 2);
    glEnd();

    glColor3f(0.2f, 0.1f, 0.0f);// Darker border color
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x - size / 2, y - size / 2);
    glVertex2f(x + size / 2, y - size / 2);
    glVertex2f(x + size / 2, y + size / 2);
    glVertex2f(x - size / 2, y + size / 2);
    glEnd();

    glBegin(GL_LINES); // For "X" pattern on the box
    glVertex2f(x - size / 2, y - size / 2); glVertex2f(x + size / 2, y + size / 2);
    glVertex2f(x + size / 2, y - size / 2); glVertex2f(x - size / 2, y + size / 2);
    glEnd();
}
//---------------------------------------------------------
// The collision with wooden boxes
//---------------------------------------------------------
void checkBoxCollision() {
    if (isInvincible) {
        invincibilityTimer--; // Decrease protection time
        if (invincibilityTimer <= 0) isInvincible = false;
        return;
    }

    for (int i = 0; i < boxes.size(); i++) {
        // Calculate distance between player and box
        float distance = sqrt(pow(boxes[i].x - playerX, 2) + pow(boxes[i].y - playerY, 2));

        if (distance < 45.0f) { // Collision threshold
            if (hearts > 0) {
                hearts--; // Lose one life
                isInvincible = true; // Temporary protection
                invincibilityTimer = 60;
            }
            boxes.erase(boxes.begin() + i); // Remove the box
            break;
        }
    }
}
//---------------------------------------------------------
// The collision with bombs (Instant Death)
//---------------------------------------------------------
void checkBombCollision() {
    for (int i = 0; i < bombs.size(); i++) {
        // Calculate the Euclidean distance between the plane and the bomb
        float distance = sqrt(pow(bombs[i].x - playerX, 2) + pow(bombs[i].y - playerY, 2));

        if (distance < 40.0f) {
            gameOver = true; // Immediate game over if hit by bomb
            break;
        }
    }
}
//---------------------------------------------------------
// Draw bomb
//---------------------------------------------------------
void drawBomb(float x, float y, float size) {
    // Shadow for depth
    glColor3f(0.0f, 0.0f, 0.0f);
    drawEllipse(x + 2, y - 2, size, size, 40);

    // Main black body
    glColor3f(0.05f, 0.05f, 0.05f);
    drawEllipse(x, y, size, size, 50);

    // Gradient layer 1
    glColor3f(0.15f, 0.15f, 0.15f);
    drawEllipse(x - size * 0.1f, y - size * 0.1f, size * 0.85f, size * 0.85f, 40);

    // Gradient layer 2 (Shining)
    glColor3f(0.25f, 0.25f, 0.25f);
    drawEllipse(x + size * 0.2f, y + size * 0.2f, size * 0.5f, size * 0.5f, 30);

    // Light reflection
    glColor3f(0.7f, 0.7f, 0.7f);
    drawEllipse(x + size * 0.4f, y + size * 0.4f, size * 0.15f, size * 0.15f, 20);

    // Top of the bomb
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(x - size * 0.2f, y + size * 0.8f);
    glVertex2f(x + size * 0.2f, y + size * 0.8f);
    glVertex2f(x + size * 0.15f, y + size * 1.1f);
    glVertex2f(x - size * 0.15f, y + size * 1.1f);
    glEnd();

    // Bomb fuse
    glColor3f(0.6f, 0.4f, 0.2f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(x, y + size * 1.1f);
    glVertex2f(x + 10, y + size * 1.3f);
    glVertex2f(x + 5, y + size * 1.5f);
    glEnd();

    // Spark at the end of the fuse
    glColor3f(1.0f, 0.8f, 0.0f);
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    glVertex2f(x + 5, y + size * 1.5f);
    glEnd();
}
//---------------------------------------------------------
// Draw the main body for the plane 
//---------------------------------------------------------
void drawPlane() {
    glPushMatrix();

    // Move to player position
    glTranslatef(playerX, playerY, 0.0f);
    // Scale plane size (Bigger)
    glScalef(30.0f, 30.0f, 1.0f);
    // Determining the Edges 
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);

    //A specific matrix to slim down the plane's body 
    //without affecting the rest of the parts.
    glPushMatrix();
    glScalef(0.8f, 1.0f, 1.0f);

    // Main body
    glBegin(GL_QUAD_STRIP);
    glColor3f(0.75f, 0.76f, 0.80f); glVertex2f(-0.50f, 2.30f); glVertex2f(-0.50f, -2.65f);
    glColor3f(1.00f, 1.00f, 1.00f); glVertex2f(0.00f, 2.30f); glVertex2f(0.00f, -2.65f);
    glColor3f(0.75f, 0.76f, 0.80f); glVertex2f(0.50f, 2.30f); glVertex2f(0.50f, -2.65f);
    glEnd();

    // Plane nose (Cone)
    glPushMatrix();
    glTranslatef(0.0f, 2.30f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glColor3f(0.95f, 0.96f, 0.98f); glutSolidCone(0.5, 1.0, 50, 50);
    glPopMatrix();

    // Cockpit
    glBegin(GL_POLYGON);
    glColor3f(0.02f, 0.05f, 0.15f); glVertex2f(0.00f, 2.90f); glVertex2f(0.25f, 2.70f);
    glColor3f(0.20f, 0.40f, 0.70f); glVertex2f(0.40f, 2.30f); glVertex2f(-0.40f, 2.30f);
    glColor3f(0.02f, 0.05f, 0.15f); glVertex2f(-0.25f, 2.70f);
    glEnd();
    // Cockpit border
    glColor3f(0.0f, 0.0f, 0.0f); glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.00f, 2.90f); glVertex2f(0.25f, 2.70f);
    glVertex2f(0.40f, 2.30f); glVertex2f(-0.40f, 2.30f);
    glVertex2f(-0.25f, 2.70f);
    glEnd();

    // Tail connector 
    glBegin(GL_QUADS);
    glColor3f(0.60f, 0.62f, 0.65f); glVertex2f(-0.25f, -2.65f); glVertex2f(0.25f, -2.65f);
    glColor3f(0.85f, 0.86f, 0.88f); glVertex2f(0.15f, -3.10f); glVertex2f(-0.15f, -3.10f);
    glEnd();
    // Tail connector border
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.25f, -2.65f); glVertex2f(0.25f, -2.65f);
    glVertex2f(0.15f, -3.10f); glVertex2f(-0.15f, -3.10f);
    glEnd();


    // Full plane body outline
    glColor3f(0.0f, 0.0f, 0.0f); glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.00f, 3.30f); glVertex2f(-0.50f, 2.30f); glVertex2f(-0.50f, -2.65f);
    glVertex2f(-0.15f, -3.10f); glVertex2f(0.15f, -3.10f); glVertex2f(0.50f, -2.65f);
    glVertex2f(0.50f, 2.30f);
    glEnd();
    glPopMatrix();

    // Draw aircraft engines
    glColor3f(0.80f, 0.82f, 0.80f);
    drawEllipse(-1.10f, 0.30f, 0.20f, 0.45f, 40);
    drawEllipse(-2.05f, -0.45f, 0.20f, 0.42f, 40);
    drawEllipse(1.10f, 0.30f, 0.20f, 0.45f, 40);
    drawEllipse(2.05f, -0.45f, 0.20f, 0.42f, 40);
    // highlights
    glColor3f(0.86f, 0.88f, 0.86f);
    drawEllipse(-1.10f, 0.30f, 0.10f, 0.30f, 30);
    drawEllipse(-1.98f, -0.45f, 0.05f, 0.26f, 30);
    drawEllipse(1.10f, 0.30f, 0.10f, 0.30f, 30);
    drawEllipse(1.98f, -0.45f, 0.05f, 0.26f, 30);
    // Draw main wings make it full of color
    glColor3f(0.86f, 0.88f, 0.86f);
    glBegin(GL_POLYGON); // Left wing
    glVertex2f(-0.28f, 0.95f); glVertex2f(-3.00f, -0.85f);
    glVertex2f(-3.15f, -1.25f); glVertex2f(-0.35f, -0.35f);
    glEnd();
    glBegin(GL_POLYGON); // Right wing
    glVertex2f(0.28f, 0.95f); glVertex2f(3.00f, -0.85f);
    glVertex2f(3.15f, -1.25f); glVertex2f(0.35f, -0.35f);
    glEnd();
    // Wing outlines
    glColor3f(0.0f, 0.0f, 0.0f); glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP); // Left wing
    glVertex2f(-0.28f, 0.95f); glVertex2f(-3.00f, -0.85f);
    glVertex2f(-3.15f, -1.25f); glVertex2f(-0.35f, -0.35f);
    glEnd();
    glBegin(GL_LINE_LOOP);// Right wing
    glVertex2f(0.28f, 0.95f); glVertex2f(3.00f, -0.85f);
    glVertex2f(3.15f, -1.25f); glVertex2f(0.35f, -0.35f);
    glEnd();


    // Blue tips at wings
    glColor3f(0.02f, 0.17f, 0.55f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-3.00f, -0.85f); glVertex2f(-3.15f, -1.25f); glVertex2f(-2.55f, -1.05f);
    glVertex2f(3.00f, -0.85f); glVertex2f(3.15f, -1.25f); glVertex2f(2.55f, -1.05f);
    glEnd();
    // Tip outlines
    glColor3f(0.0f, 0.0f, 0.0f); glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-3.00f, -0.85f); glVertex2f(-3.15f, -1.25f); glVertex2f(-2.55f, -1.05f);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex2f(3.00f, -0.85f); glVertex2f(3.15f, -1.25f); glVertex2f(2.55f, -1.05f);
    glEnd();


    // Rear tail wings
    glColor3f(0.86f, 0.88f, 0.86f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.25f, -2.05f); glVertex2f(-1.30f, -2.85f);
    glVertex2f(-1.18f, -3.20f); glVertex2f(-0.18f, -2.65f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(0.25f, -2.05f); glVertex2f(1.30f, -2.85f);
    glVertex2f(1.18f, -3.20f); glVertex2f(0.18f, -2.65f);
    glEnd();
    // Blue tips at tail 
    glColor3f(0.02f, 0.17f, 0.55f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.85f, -2.98f);
    glVertex2f(1.30f, -2.85f);  glVertex2f(1.18f, -3.20f);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.85f, -2.98f);
    glVertex2f(-1.30f, -2.85f);  glVertex2f(-1.18f, -3.20f);
    glEnd();
    // Tail outlines
    glColor3f(0.0f, 0.0f, 0.0f); glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.25f, -2.05f); glVertex2f(-1.30f, -2.85f);
    glVertex2f(-1.18f, -3.20f); glVertex2f(-0.18f, -2.65f);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.25f, -2.05f); glVertex2f(1.30f, -2.85f);
    glVertex2f(1.18f, -3.20f); glVertex2f(0.18f, -2.65f);
    glEnd();

    glPopMatrix();
}
//---------------------------------------------------------
// Win screen helper functions
//---------------------------------------------------------
void drawTextWin(float x, float y, const char* text, void* font) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(font, text[i]);
    }
}
void drawCloudWin(float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f); // White color for clouds
    drawEllipse(x, y, 0.12f, 0.05f, 40);
    drawEllipse(x + 0.10f, y + 0.02f, 0.12f, 0.06f, 40);
    drawEllipse(x + 0.22f, y, 0.13f, 0.05f, 40);
}
void drawStarWin(float x, float y) {
    glColor3f(1.0f, 0.95f, 0.65f); // Soft yellow for stars
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 0.04f); glVertex2f(x - 0.04f, y - 0.03f); glVertex2f(x + 0.04f, y - 0.03f);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y - 0.04f); glVertex2f(x - 0.04f, y + 0.03f); glVertex2f(x + 0.04f, y + 0.03f);
    glEnd();
}
//---------------------------------------------------------
// Final win screen 
//---------------------------------------------------------

void drawWinScreen() {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup 2D Orthographic Projection for UI
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Blue sky gradient background
    glBegin(GL_QUADS);
    glColor3f(0.05f, 0.05f, 0.35f); //Dark blue
    glVertex2f(-1, 1); glVertex2f(1, 1);
    glColor3f(0.55f, 0.82f, 1.0f);// Light blue 
    glVertex2f(1, -1); glVertex2f(-1, -1);
    glEnd();

    // Stars and clouds
    drawStarWin(-0.85f, 0.82f);
    drawStarWin(-0.55f, 0.72f);
    drawStarWin(-0.30f, 0.86f);
    drawStarWin(0.10f, 0.77f);
    drawStarWin(0.45f, 0.84f);
    drawStarWin(0.70f, 0.70f);
    drawStarWin(0.88f, 0.80f);

    drawCloudWin(-0.78f, 0.38f);
    drawCloudWin(0.05f, 0.48f);
    drawCloudWin(0.55f, 0.40f);

    // Winning message box
    glColor3f(0.02f, 0.02f, 0.20f); drawRect(-0.72f, 0.35f, 0.72f, -0.10f);
    glColor3f(1.0f, 0.85f, 0.05f); drawRectBorder(-0.72f, 0.35f, 0.72f, -0.10f); // Golden border
    glColor3f(1.0f, 0.85f, 0.15f); drawTextWin(-0.28f, 0.17f, "YOU WIN!", GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.75f, 0.85f, 1.0f); drawTextWin(-0.55f, 0.03f, "Congratulations! You Achieve  100 Score.", GLUT_BITMAP_HELVETICA_18);

    // Restart button box
    glColor3f(0.02f, 0.02f, 0.20f); drawRect(-0.55f, -0.30f, 0.55f, -0.55f);
    glColor3f(0.0f, 1.0f, 0.0f); drawTextWin(-0.22f, -0.45f, "Press R to Restart", GLUT_BITMAP_HELVETICA_18);

    glColor3f(1.0f, 1.0f, 1.0f);
}
//---------------------------------------------------------
// Draw Environment (Sky and Ground textures)
//---------------------------------------------------------
void drawEnviroment() {
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    // Draw moving sky texture
    glBindTexture(GL_TEXTURE_2D, skyTex);
    glBegin(GL_QUADS);
    // At first 4 seconds, sky texture will be static
    glTexCoord2f(0.0f, 0.0f + skyMove); glVertex2f(0, 0);
    glTexCoord2f(2.0f, 0.0f + skyMove); glVertex2f(width, 0);
    glTexCoord2f(2.0f, 2.0f + skyMove); glVertex2f(width, height);
    glTexCoord2f(0.0f, 2.0f + skyMove); glVertex2f(0, height);
    glEnd();

    // Draw ground (appears only at the start)
    if (groundMove < 250.0f) {
        glBindTexture(GL_TEXTURE_2D, groundTex);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);  glVertex2f(0, 0);
        glTexCoord2f(3.0f, 0.0f);  glVertex2f(width, 0);
        glTexCoord2f(3.0f, 0.6f);  glVertex2f(width, 120);// Zoom-in
        glTexCoord2f(0.0f, 0.6f);  glVertex2f(0, 120);
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}
// Countdown font
void renderHugeText(float x, float y, std::string text, float scale, float thickness) {
    glPushMatrix();

    // 1. Set text position
    glTranslatef(x, y, 0);

    // 2. Control text size (the scale parameter passed)
    glScalef(scale, scale, 1.0f);

    // 3. Control line thickness (the thickness parameter passed)
    glLineWidth(thickness);

    // Draw letters
    for (char c : text) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    }

    // Reset line thickness to default so it doesn't affect the rest of the game rendering
    glLineWidth(1.0f);

    glPopMatrix();
}

// Game Over screen function
void drawGameOverScreen() {
    // 1. Tint the screen with a light red overlay as a background
    glColor4f(0.2f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0); glVertex2f(width, 0);
    glVertex2f(width, height); glVertex2f(0, height);
    glEnd();

    // 2. "GAME OVER" text in red with a large font size
    glColor3f(1.0f, 0.0f, 0.0f);
    // (Width is 600, so we offset the text slightly left to center it)
    renderHugeText(width / 2 - 200, height / 2 + 50, "GAME OVER", 0.5f, 8.0f);

    // 3. Restart message in white color underneath
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(width / 2 - 100, height / 2 - 50, "Press 'R' to Restart", GLUT_BITMAP_HELVETICA_18);
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (scene == 0) {

        drawStartScreen();
    }
    else if (gameWin) {

        drawWinScreen();
    }
    else {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, width, 0, height);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        drawEnviroment();

        if (!gameOver) {
            // Draw the airplane and objects while alive
            if (!isInvincible || (invincibilityTimer % 10 > 5)) drawPlane();
            for (auto& b : boxes) drawWoodBox(b.x, b.y, b.size);
            for (auto& b : bombs) drawBomb(b.x, b.y, b.size);

            // --- Draw UI (Score and Hearts)---
            glColor3f(1.0f, 1.0f, 1.0f);
            renderText(20, height - 40, "Score: " + std::to_string(score));
            renderText(width / 2 - 40, height - 40, "Level: " + std::to_string(gameLevel));

            for (int i = 0; i < hearts; i++) {
                drawHeart(width - 40 - (i * 35), height - 35, 20);
            }

            // Draw the countdown timer (3-2-1)
            if (groundMove < 250.0f) {
                glColor3f(1, 1, 0);
                if (groundMove < 80.0f) renderHugeText(width / 2 - 30, height / 2, "3", 0.4f, 5.0f);
                else if (groundMove < 160.0f) renderHugeText(width / 2 - 30, height / 2, "2", 0.4f, 5.0f);
                else if (groundMove < 240.0f) renderHugeText(width / 2 - 30, height / 2, "1", 0.4f, 5.0f);
            }
        }
        else {
            drawGameOverScreen();
        }
    }
    glutSwapBuffers();
}



// --- Level Logic & Update ---
void updateLevelSettings() {
    if (difficulty == 0) { gameLevel = 1; currentSpeed = 4.0f; spawnRate = 80; }
    else if (difficulty == 1) { gameLevel = 2; currentSpeed = 6.5f; spawnRate = 50; }
    else if (difficulty == 2) { gameLevel = 3; currentSpeed = 9.0f; spawnRate = 35; }
}


void update(int value) {
    if (scene == 1 && !gameOver && !gameWin) {
        updateLevelSettings();
        if (score >= 100) gameWin = true;
        if (groundMove < 250.0f) groundMove += 1.0f;
        else {
            skyMove += 0.002f;
            if (rand() % spawnRate == 0) {
                WoodBox b; b.x = rand() % (width - 60) + 30; b.y = height + 50;
                b.size = 40.0f; b.speed = currentSpeed; boxes.push_back(b);
            }
            if (gameLevel == 3 && rand() % 150 == 0) {
                Bomb b; b.x = rand() % (width - 60) + 30; b.y = height + 50;
                b.size = 25.0f; b.speed = currentSpeed + 2.0f; bombs.push_back(b);
            }
        }
        for (auto it = boxes.begin(); it != boxes.end(); ) {
            it->y -= it->speed;
            if (it->y < -50) { it = boxes.erase(it); score++; }
            else ++it;
        }
        for (auto it = bombs.begin(); it != bombs.end(); ) {
            it->y -= it->speed;
            if (it->y < -50) it = bombs.erase(it); else ++it;
        }
        checkBoxCollision();
        checkBombCollision();
        if (hearts <= 0) gameOver = true;
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}


// --- Controls ---
void specialKeys(int key, int x, int y) {
    if (!gameOver) {
        if (key == GLUT_KEY_LEFT && playerX > 50) playerX -= playerSpeed;
        if (key == GLUT_KEY_RIGHT && playerX < width - 50) playerX += playerSpeed;
        if (key == GLUT_KEY_UP && playerY < height - 100) playerY += playerSpeed;
        if (key == GLUT_KEY_DOWN && playerY > 60) playerY -= playerSpeed;
    }
}

void normalKeys(unsigned char key, int x, int y) {
    if ((key == 'r' || key == 'R') && (gameOver || gameWin)) {
        gameOver = false;
        gameWin = false;   // Now the win screen will disappear
        scene = 1;
        score = 0;
        hearts = 3;
        isInvincible = false;
        invincibilityTimer = 0;

        // Reset the airplane to its original position
        playerX = 305.0f;
        playerY = 60.0f;

        // Reset counter and environment
        groundMove = 0.0f;
        skyMove = 0.0f;

        // Clear old boxes from the screen
        boxes.clear();
        bombs.clear();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, width, 0, height);
        glMatrixMode(GL_MODELVIEW);
        // Request an immediate screen redraw
        glutPostRedisplay();
        std::cout << "Game Restarted from Win/Loss state!" << std::endl;
    }
    if ((key == 'x' || key == 'X') && scene == 0)
        scene = 1;
    score = 0;
    groundMove = 0.0f;
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && scene == 0) {
        float nx = (2.0f * x) / width - 1.0f;
        float ny = 1.0f - (2.0f * y) / height;
        if (ny >= -0.76f && ny <= -0.64f) {
            if (nx >= -0.50f && nx <= -0.20f) difficulty = 0;
            else if (nx >= -0.15f && nx <= 0.15f) difficulty = 1;
            else if (nx >= 0.20f && nx <= 0.50f) difficulty = 2;
        }
        glutPostRedisplay();
    }
}

// --- Run Program ---
GLuint LoadTexture(const char* filename, int width, int height) {
    GLuint texture;
    unsigned char* data;
    FILE* file = fopen(filename, "rb");

    if (file == NULL) return -1;

    data = (unsigned char*)malloc(width * height * 3);
    fread(data, width * height * 3, 1, file);
    fclose(file);

    // Convert color order from BGR to RGB
    for (int i = 0; i < width * height; ++i) {
        int index = i * 3;
        unsigned char B = data[index], R = data[index + 2];
        data[index] = R; data[index + 2] = B;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //loop the texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    free(data);
    return texture;
}
int main(int argc, char** argv) {
    srand(time(0));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("GALAGA X");

    groundTex = LoadTexture(groundTexPath, 512, 512);
    skyTex = LoadTexture(skyTexPath, 512, 512);


    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.01f, 0.01f, 0.03f, 1.0f);
    glutDisplayFunc(display); glutSpecialFunc(specialKeys); glutKeyboardFunc(normalKeys);
    glutMouseFunc(mouse);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}