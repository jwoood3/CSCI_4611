/** CSci-4611 Assignment 3:  Earthquake
* Jeremy Wood
 */

#include "earth.h"
#include "config.h"

#include <vector>

// for M_PI constant
#define _USE_MATH_DEFINES
#include <math.h>

std::vector<unsigned int> indices;
std::vector<Point3> vertices;
std::vector<Vector3> normals;
std::vector<Point2> tex_coords;

std::vector<Point3> sphereVertices;
std::vector<Vector3> sphereNormals;


Earth::Earth() {
}

Earth::~Earth() {
}

void Earth::Init(const std::vector<std::string> &search_path) {
    // init shader program
    shader_.Init();
    // init texture: you can change to a lower-res texture here if needed
    earth_tex_.InitFromFile(Platform::FindFile("earth-2k.png", search_path));

    // init geometry
    //Number of mesh rows and columns
    //To change the mesh rows and columns, only nslices and nstacks need to be changed, everything else is dynamic
    const int nslices = 20;
    const int nstacks = 20;

    //Creating the vertices for the mesh requires floating point numbers. 
    //Could use a cast instead, but then there would need to be multiple casts so this is easier
    float nslicesFloat = nslices;
    float nstacksFloat = nstacks;

    // TODO: This is where you need to set the vertices and indiceds for earth_mesh_.

    //Create the plane and sphere vertices as well as the plane and sphere normals
    //Goes fully across a row, then down to the next column
    //The dimensions of the plane are -pi to pi in the x direction, and pi/2 to -pi/2 in the y direction (with pi/2 being the top)
    for (float q = 0.0; q <= nstacksFloat; q++) {
        for (float r = 0.0; r <= nslicesFloat; r++) {
            vertices.push_back(Point3((((2 * r) - nslicesFloat) / nslicesFloat) * M_PI, ((nstacksFloat - (2 * q)) / (2 * nstacksFloat)) * M_PI, 0));
            normals.push_back(Vector3(0, 0, 1).ToUnit());

            Point3 sphere_coord = LatLongToSphere(90 - (q / nstacksFloat) * 180, (r / nslicesFloat) * 360 - 180);
            sphereVertices.push_back(sphere_coord);
            sphereNormals.push_back((sphere_coord - Point3(0, 0, 0)).ToUnit());
        }
    }

    //Set indices by looping through nSlices and nStacks
    //This is what creates the triangles of the mesh
    //Each square of the mesh has two triangle, one is moreso on the bottom right, the other moreso on the top left
    for (int i = 0; i < nstacks; i++) {
        for (int j = 0; j < nslices; j++) {
            //bottom right triangle
            indices.push_back(j + (i * (nslices + 1)));
            indices.push_back(j + (i * (nslices + 1)) + (nslices + 1));
            indices.push_back(j + (i * (nslices + 1)) + (nslices + 2));

            //bottom left triangle
            indices.push_back(j + (i * (nslices + 1)));
            indices.push_back(j + (i * (nslices + 1)) + (nslices + 2));
            indices.push_back(j + (i * (nslices + 1)) + 1);
        }
    }

    //Set the texture coordinates so that the image of the earth appears over the plane/sphere
    for (int s = 0; s <= nstacks; s++) {
        for (int t = 0; t <= nslices; t++) {
            tex_coords.push_back(Point2(t/nslicesFloat, s/nstacksFloat));
        }
    }
    
    earth_mesh_.SetVertices(vertices);
    earth_mesh_.SetIndices(indices);
    earth_mesh_.SetTexCoords(0, tex_coords);
    earth_mesh_.UpdateGPUMemory();
}



void Earth::Draw(const Matrix4 &model_matrix, const Matrix4 &view_matrix, const Matrix4 &proj_matrix) {
    // Define a really bright white light.  Lighting is a property of the "shader"
    DefaultShader::LightProperties light;
    light.position = Point3(10,10,10);
    light.ambient_intensity = Color(1,1,1);
    light.diffuse_intensity = Color(1,1,1);
    light.specular_intensity = Color(1,1,1);
    shader_.SetLight(0, light);

    // Adust the material properties, material is a property of the thing
    // (e.g., a mesh) that we draw with the shader.  The reflectance properties
    // affect the lighting.  The surface texture is the key for getting the
    // image of the earth to show up.
    DefaultShader::MaterialProperties mat;
    mat.ambient_reflectance = Color(0.5, 0.5, 0.5);
    mat.diffuse_reflectance = Color(0.75, 0.75, 0.75);
    mat.specular_reflectance = Color(0.75, 0.75, 0.75);
    mat.surface_texture = earth_tex_;

    // Draw the earth mesh using these settings
    if (earth_mesh_.num_triangles() > 0) {
        shader_.Draw(model_matrix, view_matrix, proj_matrix, &earth_mesh_, mat);
    }
}


Point3 Earth::LatLongToSphere(double latitude, double longitude) const {
    // TODO: We recommend filling in this function to put all your
    // lat,long --> sphere calculations in one place.

    float latRad = GfxMath::ToRadians(latitude);
    float longRad = GfxMath::ToRadians(longitude);

    float x = cos(latRad) * sin(longRad);
    float y = sin(latRad);
    float z = cos(latRad) * cos(longRad);

    return Point3(x, y, z);
}

Point3 Earth::LatLongToPlane(double latitude, double longitude) const {
    // TODO: We recommend filling in this function to put all your
    // lat,long --> plane calculations in one place.

    float x = (float) longitude * (M_PI / 180);
    float y = (float) latitude * (M_PI / 180);

    return Point3(x,y,0);
}

/// <summary>
/// Transitions from plane to sphere and back when globe button is pressed
/// </summary>
/// <param name="sphere"> transition variable from quake_app.cc, denotes how far through the transition is </param>
void Earth::PlaneSphereTransition(float sphere) {
    //Everything in this method is inside the if statement. If the transition variable which is passed in for sphere
    //is at exactly 0.0, then the mesh is already a plane. If it is at 1.0, then the mesh is already a sphere.
    //If it is at either of these, there is nothing to be done
    if (sphere != 0.0 && sphere != 1.0) {
        std::vector<Point3> transitionVertices;
        std::vector<Vector3> transitionNormals;

        for (int i = 0; i < vertices.size(); i++) {
            //Linear interpolation of the plane and sphere vertices for smooth transition between the two
            transitionVertices.push_back(Point3(
                GfxMath::Lerp(vertices[i].x(), sphereVertices[i].x(), sphere),
                GfxMath::Lerp(vertices[i].y(), sphereVertices[i].y(), sphere),
                GfxMath::Lerp(vertices[i].z(), sphereVertices[i].z(), sphere)
            ));

            //Linear interpolation of the plane and sphere normals for smooth transition of normals which allows a smooth lighting transition
            transitionNormals.push_back(Vector3(
                GfxMath::Lerp(normals[i].x(), sphereNormals[i].x(), sphere),
                GfxMath::Lerp(normals[i].y(), sphereNormals[i].y(), sphere),
                GfxMath::Lerp(normals[i].z(), sphereNormals[i].z(), sphere)
            ));
        }

        earth_mesh_.SetVertices(transitionVertices);
        earth_mesh_.SetNormals(transitionNormals);

        earth_mesh_.UpdateGPUMemory();
    }
}


void Earth::DrawDebugInfo(const Matrix4 &model_matrix, const Matrix4 &view_matrix, const Matrix4 &proj_matrix) {
    // This draws a cylinder for each line segment on each edge of each triangle in your mesh.
    // So it will be very slow if you have a large mesh, but it's quite useful when you are
    // debugging your mesh code, especially if you start with a small mesh.
    for (int t=0; t<earth_mesh_.num_triangles(); t++) {
        std::vector<unsigned int> indices = earth_mesh_.triangle_verticesReadOnly(t);
        std::vector<Point3> loop;
        loop.push_back(earth_mesh_.vertexReadOnly(indices[0]));
        loop.push_back(earth_mesh_.vertexReadOnly(indices[1]));
        loop.push_back(earth_mesh_.vertexReadOnly(indices[2]));
        quick_shapes_.DrawLines(model_matrix, view_matrix, proj_matrix,
            Color(1,1,0), loop, QuickShapes::LinesType::LINE_LOOP, 0.005);
    }
}
