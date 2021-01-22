/** CSci-4611 Assignment 2:  Car Soccer
 */

#include "car_soccer.h"
#include "config.h"


CarSoccer::CarSoccer() : GraphicsApp(1024,768, "Car Soccer") {
    // Define a search path for finding data files (images and shaders)
    searchPath_.push_back(".");
    searchPath_.push_back("./data");
    searchPath_.push_back(DATA_DIR_INSTALL);
    searchPath_.push_back(DATA_DIR_BUILD);
}

CarSoccer::~CarSoccer() {
}


Vector2 CarSoccer::joystick_direction() {
    Vector2 dir;
    if (IsKeyDown(GLFW_KEY_LEFT))
        dir[0]--;
    if (IsKeyDown(GLFW_KEY_RIGHT))
        dir[0]++;
    if (IsKeyDown(GLFW_KEY_UP))
        dir[1]++;
    if (IsKeyDown(GLFW_KEY_DOWN))
        dir[1]--;
    return dir;
}


void CarSoccer::OnSpecialKeyDown(int key, int scancode, int modifiers) {
    if (key == GLFW_KEY_SPACE) {
        // Here's where you could call some form of launch_ball();
		ball_.Reset();
		launch_ball();
    }
}


void CarSoccer::UpdateSimulation(double timeStep) {
    // Here's where you shound do your "simulation", updating the positions of the
    // car and ball as needed and checking for collisions.  Filling this routine
    // in is the main part of the assignment.
	updateBallPos(timeStep);
	updateCarPos(timeStep);
}


void CarSoccer::InitOpenGL() {
    // Set up the camera in a good position to see the entire field
    projMatrix_ = Matrix4::Perspective(60, aspect_ratio(), 1, 1000);
    modelMatrix_ = Matrix4::LookAt(Point3(0,60,70), Point3(0,0,10), Vector3(0,1,0));
 
    // Set a background color for the screen
    glClearColor(0.8,0.8,0.8, 1);
    
    // Load some image files we'll use
    fieldTex_.InitFromFile(Platform::FindFile("pitch.png", searchPath_));
    crowdTex_.InitFromFile(Platform::FindFile("crowd.png", searchPath_));
}


void CarSoccer::DrawUsingOpenGL() {
    // Draw the crowd as a fullscreen background image
    quickShapes_.DrawFullscreenTexture(Color(1,1,1), crowdTex_);
    
    // Draw the field with the field texture on it.
    Color col(16.0/255.0, 46.0/255.0, 9.0/255.0);
    Matrix4 M = Matrix4::Translation(Vector3(0,-0.201,0)) * Matrix4::Scale(Vector3(50, 1, 60));
    quickShapes_.DrawSquare(modelMatrix_ * M, viewMatrix_, projMatrix_, col);
    M = Matrix4::Translation(Vector3(0,-0.2,0)) * Matrix4::Scale(Vector3(40, 1, 50));
    quickShapes_.DrawSquare(modelMatrix_ * M, viewMatrix_, projMatrix_, Color(1,1,1), fieldTex_);
    
    // Draw the car
    Color carcol(0.8, 0.2, 0.2);
    Matrix4 Mcar =
        Matrix4::Translation(car_.position() - Point3(0,0,0)) *
		Matrix4::RotationY(GfxMath::ToDegrees(car_.carAngle())) *
        Matrix4::Scale(car_.size()) *
        Matrix4::Scale(Vector3(0.5,0.5,0.5));
    quickShapes_.DrawCube(modelMatrix_ * Mcar, viewMatrix_, projMatrix_, carcol);
    
    
    // Draw the ball
    Color ballcol(1,1,1);
    Matrix4 Mball =
        Matrix4::Translation(ball_.position() - Point3(0,0,0)) *
        Matrix4::Scale(Vector3(ball_.radius(), ball_.radius(), ball_.radius()));
    quickShapes_.DrawSphere(modelMatrix_ * Mball, viewMatrix_, projMatrix_, ballcol);
    
    
    // Draw the ball's shadow -- this is a bit of a hack, scaling Y by zero
    // flattens the sphere into a pancake, which we then draw just a bit
    // above the ground plane.
    Color shadowcol(0.2,0.4,0.15);
    Matrix4 Mshadow =
        Matrix4::Translation(Vector3(ball_.position()[0], -0.1, ball_.position()[2])) *
        Matrix4::Scale(Vector3(ball_.radius(), 0, ball_.radius())) *
        Matrix4::RotationX(90);
    quickShapes_.DrawSphere(modelMatrix_ * Mshadow, viewMatrix_, projMatrix_, shadowcol);
    
    
    // You should add drawing the goals and the boundary of the playing area
    // using quickShapes_.DrawLines()
	drawBounds();
	drawGoal();
}

///Launches the ball when a reset occurs
void CarSoccer::launch_ball() {
	float randXVelocity = (rand() % 6) - 3; //Random between -3 and 3
	float randYVelocity = 30 + rand() % 10; //Random between 30 and 40
	float randZVelocity = (rand() % 6) - 3; //Random between -3 and 3
	//Ball initial velocity is set low in the x and z directions so it does not go too far from the start
	Vector3 initialVelocity = Vector3(randXVelocity, randYVelocity, randZVelocity);
	ball_.set_ballVelocity(initialVelocity);
}

///Draws the boundary lines
void CarSoccer::drawBounds() {
	Color boundscol(0.7, 0.2, 0.3);
	std::vector<Point3> loop1;
	loop1.push_back(Point3(40, 0, 50));
	loop1.push_back(Point3(-40, 0, 50));
	loop1.push_back(Point3(-40, 0, -50));
	loop1.push_back(Point3(40, 0, -50));
	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, boundscol, loop1, QuickShapes::LinesType::LINE_LOOP, 0.1);

	std::vector<Point3> loop2;
	loop2.push_back(Point3(40, 35, 50));
	loop2.push_back(Point3(-40, 35, 50));
	loop2.push_back(Point3(-40, 35, -50));
	loop2.push_back(Point3(40, 35, -50));
	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, boundscol, loop2, QuickShapes::LinesType::LINE_LOOP, 0.1);

	std::vector<Point3> loop3;
	loop3.push_back(Point3(40, 0, 50));
	loop3.push_back(Point3(40, 35, 50));
	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, boundscol, loop3, QuickShapes::LinesType::LINE_LOOP, 0.1);

	std::vector<Point3> loop4;
	loop4.push_back(Point3(-40, 0, -50));
	loop4.push_back(Point3(-40, 35, -50));
	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, boundscol, loop4, QuickShapes::LinesType::LINE_LOOP, 0.1);

	std::vector<Point3> loop5;
	loop5.push_back(Point3(-40, 0, 50));
	loop5.push_back(Point3(-40, 35, 50));
	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, boundscol, loop5, QuickShapes::LinesType::LINE_LOOP, 0.1);

	std::vector<Point3> loop6;
	loop6.push_back(Point3(40, 0, -50));
	loop6.push_back(Point3(40, 35, -50));
	quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, boundscol, loop6, QuickShapes::LinesType::LINE_LOOP, 0.1);
}

///Draws the goals
void CarSoccer::drawGoal() {
	//Orange goal on Player's side
	Color goal1Color(0.988, 0.408, 0.05);
	for (int hLines1 = 0; hLines1 <= 10; hLines1++) {
		std::vector<Point3> goal11;
		goal11.push_back(Point3(-10, hLines1, 50));
		goal11.push_back(Point3(10, hLines1, 50));
		quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, goal1Color, goal11, QuickShapes::LinesType::LINE_LOOP, 0.1);
	}
	for (int vLines1 = -10; vLines1 <= 10; vLines1++) {
		std::vector<Point3> goal12;
		goal12.push_back(Point3(vLines1, 0, 50));
		goal12.push_back(Point3(vLines1, 10, 50));
		quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, goal1Color, goal12, QuickShapes::LinesType::LINE_LOOP, 0.1);
	}
	
	//Blue goal on opposing side
	Color goal2Color(0, 0.7, 0.9);
	for (int hLines2 = 0; hLines2 <= 10; hLines2++) {
		std::vector<Point3> goal21;
		goal21.push_back(Point3(-10, hLines2, -50));
		goal21.push_back(Point3(10, hLines2, -50));
		quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, goal2Color, goal21, QuickShapes::LinesType::LINE_LOOP, 0.1);
	}
	for (int vLines = -10; vLines <= 10; vLines++) {
		std::vector<Point3> goal22;
		goal22.push_back(Point3(vLines, 0, -50));
		goal22.push_back(Point3(vLines, 10, -50));
		quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, goal2Color, goal22, QuickShapes::LinesType::LINE_LOOP, 0.1);
	}
}

///Updates the position of the ball, does boundary checking, collision detection/handling, goal checking, and gravity
void CarSoccer::updateBallPos(double timeStep) {
	//Gets the current position and velocity of the ball at the beginning of the timeStep
	Point3 currentPosition = ball_.position();
	Vector3 currentVelocity = ball_.ballVelocity();

	//Updates the ball's position based on the velocity
	Point3 updatePosition = currentPosition + timeStep * currentVelocity;
	
	//If the ball hits the x wall (left or right)
	if (abs(updatePosition.x()) + ball_.radius() >= 40.0) {
		updatePosition[0] = updatePosition.x() < 0 ? -40 + ball_.radius() : 40 - ball_.radius();
		currentVelocity[0] = -0.5 * currentVelocity.x();
		currentVelocity[1] = 0.9 * currentVelocity.y();
		currentVelocity[2] = 0.9 * currentVelocity.z();
	}
	//If the ball hits the ceiling
	if (updatePosition.y() + ball_.radius() >= 35.0) {
		updatePosition[1] = 35 - ball_.radius();
		currentVelocity[1] = -0.5 * currentVelocity.y();
		currentVelocity[0] = 0.9 * currentVelocity.x();
		currentVelocity[2] = 0.9 * currentVelocity.z();
	}
	//If the ball hits the z wall (front or back)
	if (abs(updatePosition.z()) + ball_.radius() >= 50.0) {
		updatePosition[2] = updatePosition.z() < 0 ? -50 + ball_.radius() : 50 - ball_.radius();
		currentVelocity[2] = -0.5 * currentVelocity.z();
		currentVelocity[1] = 0.9 * currentVelocity.y();
		currentVelocity[0] = 0.9 * currentVelocity.x();
	}
	//If ball hits ground
	if (updatePosition.y() - ball_.radius() <= 0) {
		updatePosition[1] = ball_.radius();
		currentVelocity[1] = -0.5 * currentVelocity.y();
		currentVelocity[0] = 0.95 * currentVelocity.x();
		currentVelocity[2] = 0.95 * currentVelocity.z();
	}
	//If ball hits car
	collisionHandling();

	ball_.set_position(updatePosition);

	//If the ball goes in a goal
	if (abs(updatePosition.x()) + ball_.radius() <= 10 && abs(updatePosition.z()) + ball_.radius() >= 49 && updatePosition.y() + ball_.radius() <= 10) {
		ball_.Reset();
		ball_.set_ballVelocity(Vector3(0, 0, 0));
		//currentVelocity = resetVelocity;
		//should reset velocity in here
		launch_ball();
		car_.Reset();
		return;
	}

	//Changes the velocity of the ball (only on the y axis) due to gravity
	Vector3 gravity = Vector3(0, 0.05, 0);
	currentVelocity = currentVelocity - gravity;
	ball_.set_ballVelocity(currentVelocity);
}

///Updates the position of the car, does boundary checking, friction, turning the car, also does collision detection/handling
void CarSoccer::updateCarPos(double timeStep) {
	//Gets the current position and velocity of the car at the beginning of the timeStep
	Point3 currentCarPosition = car_.position();
	Vector3 currentCarVelocity = car_.carVelocity();
	//Part of the friction for the car, slows it down so it will stop if nothing is pressed
	currentCarVelocity = currentCarVelocity * 0.98;

	Point3 updateCarPosition = currentCarPosition + timeStep * currentCarVelocity;
	//If the car hits the x wall (left or right)
	if (abs(updateCarPosition.x()) + car_.collision_radius() >= 40.0) {
		updateCarPosition[0] = updateCarPosition.x() < 0 ? -40 + car_.collision_radius() : 40 - car_.collision_radius();
		currentCarVelocity[0] = -0.05 * currentCarVelocity.x();
		//currentCarVelocity[1] = 0.9 * currentCarVelocity.y();
		currentCarVelocity[2] = 0.9 * currentCarVelocity.z();
	}
	//If the car hits the z wall (front or back)
	if (abs(updateCarPosition.z()) + car_.collision_radius() >= 50.0) {
		updateCarPosition[2] = updateCarPosition.z() < 0 ? -50 + car_.collision_radius() : 50 - car_.collision_radius();
		currentCarVelocity[2] = -0.05 * currentCarVelocity.z();
		//currentCarVelocity[1] = 0.9 * currentCarVelocity.y();
		currentCarVelocity[0] = 0.9 * currentCarVelocity.x();
	}
	//If car hits ball
	collisionHandling();

	car_.set_position(updateCarPosition);

	Vector2 dir = joystick_direction();
	//If statements that do nothing are left in for clarity
	if (car_.carSpeed() > 1 && dir.y() < 0) {
		//do not increment car speed
	}
	else if (car_.carSpeed() > 1 && dir.y() > 0) {
		car_.set_carSpeed(car_.carSpeed() - dir.y() / 20);
	}
	else if (car_.carSpeed() < -1 && dir.y() > 0) {
		//do not increment
	}
	else if (car_.carSpeed() < -1 && dir.y() < 0) {
		car_.set_carSpeed(car_.carSpeed() - dir.y() / 20);
	}
	else {
		car_.set_carSpeed(car_.carSpeed() - dir.y() / 20);
	}

	//Turn the Vector2 into a Vector3
	//The x component of velocity is multiplied by sin of the angle, z component by cos so that the car moves forward in the direction it is facing
	Vector3 dirToVector3;
	dirToVector3[0] += sin(GfxMath::ToDegrees(car_.carAngle())) * car_.carSpeed();//0;//+= (abs(dirToVector3[0]) > 5 ? dirToVector3[0] : dir.x()/2);
	dirToVector3[1] = 0;
	dirToVector3[2] += cos(GfxMath::ToDegrees(car_.carAngle())) * car_.carSpeed();//+= (abs(dirToVector3[2]) > 5 ? dirToVector3[2] : -dir.y()/2);
	
	//update the velocity
	currentCarVelocity = currentCarVelocity + dirToVector3;
	car_.set_carVelocity(currentCarVelocity);

	//Another part of the friction of the car
	if (dir.y() == 0) {
		car_.set_carSpeed(0);
	}

	//Gets the angle based on arrow key pressed from dir
	float angle = car_.carAngle() - dir.x()/2000;
	car_.set_carAngle(angle);
	
}

///Checks for a collision between the car and the ball
bool CarSoccer::isCollision() {
	//Distance between the center of the ball and car
	float distance = sqrt(pow((ball_.position().x() - car_.position().x()), 2) + pow((ball_.position().y() - car_.position().y()), 2) + pow((ball_.position().z() - car_.position().z()), 2));
	float radii = ball_.radius() + car_.collision_radius();
	//Can also be written as (distance <= radii). This indicates if the ball and car collision spheres touch or overlap
	if (distance - radii <= 0) {
		return true;
	}
	else {
		return false;
	}
}

///Handles a collision between the car and the ball by "bouncing" the ball off of the car
void CarSoccer::collisionHandling() {
	if (isCollision()) {
		//Normal vector of ball and car collision
		Vector3 n = ball_.position() - car_.position();
		n.Normalize();
		//set the ball to be just outside of the radius of the car
		ball_.set_position(car_.position() + (ball_.radius() + car_.collision_radius()) * n);
		//Set and reflect relative velocity around collision normal, and set the ball's velocity based on that
		Vector3 relVelocity = ball_.ballVelocity() - car_.carVelocity();
		relVelocity = relVelocity - 2 * relVelocity.Dot(n) * n;
		ball_.set_ballVelocity(relVelocity + car_.carVelocity());
	}
}