/**
This application renders a textured mesh that was loaded with Assimp.
*/

#include <iostream>
#include <memory>

#include "Mesh3D.h"
#include "Object3D.h"
#include "AssimpImport.h"
#include "Animator.h"
#include "ShaderProgram.h"
#include "OrbitalAnimation.h"

/**
 * @brief Defines a collection of objects that should be rendered with a specific shader program.
 */
struct Scene {
	ShaderProgram defaultShader;
	std::vector<Object3D> objects;
	std::vector<Animator> animators;
};

/**
 * @brief Constructs a shader program that renders textured meshes in the Phong reflection model.
 * The shaders used here are incomplete; see their source codes.
 * @return 
 */
ShaderProgram phongLighting() {
	ShaderProgram program;
	try {
		program.load("shaders/light_perspective.vert", "shaders/lighting.frag");
	}
	catch (std::runtime_error& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		exit(1);
	}
	return program;
}

/**
 * @brief Constructs a shader program that renders textured meshes without lighting.
 */
ShaderProgram textureMapping() {
	ShaderProgram program;
	try {
		program.load("shaders/texture_perspective.vert", "shaders/texturing.frag");
	}
	catch (std::runtime_error& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		exit(1);
	}
	return program;
}

/**
 * @brief Loads an image from the given path into an OpenGL texture.
 */
Texture loadTexture(const std::filesystem::path& path, const std::string& samplerName = "baseTexture") {
	sf::Image i;
	i.loadFromFile(path.string());
	return Texture::loadImage(i, samplerName);
}

/**
 * @brief  Demonstrates loading a square, oriented as the "floor", with a manually-specified texture
 * that does not come from Assimp.
 * @return 
 */
Scene marbleSquare() {
	std::vector<Texture> textures = {
		loadTexture("models/White_marble_03/Textures_4K/white_marble_03_4k_baseColor.tga", "baseTexture"),
	};

	auto mesh = Mesh3D::square(textures);
	auto square = Object3D(std::vector<Mesh3D>{mesh});
	square.grow(glm::vec3(5, 5, 5));
	square.rotate(glm::vec3(-3.14159 / 4, 0, 0));
	return Scene{
		phongLighting(),
		{square}
	};
}

/**
 * @brief Constructs a scene of the textured Stanford bunny.
 */
Scene bunny() {
	auto bunny = assimpLoad("models/bunny_textured.obj", true);
	bunny.grow(glm::vec3(9, 9, 9));
	bunny.move(glm::vec3(0.2, -1, 0));

	return Scene{
		phongLighting(),
		{bunny}
	};
}

/**
 * @brief Constructs a scene of a tiger sitting in a boat, where the tiger is the child object
 * of the boat.
 * @return 
 */
Scene lifeOfPi() {
	// This scene is more complicated; it has child objects, as well as animators.
	auto sun = assimpLoad("models/Venus_1K.obj", true);
	sun.move(glm::vec3(0, 0, 0));
	sun.grow(glm::vec3(2.0, 2.0, 2.0));
	
	auto mercury = assimpLoad("models/Mercury_1K.obj", true);
	mercury.move(glm::vec3(5, 0, 5));
	mercury.grow(glm::vec3(1.3, 1.3, 1.3));
	//sun.addChild(std::move(mercury));
	
	auto venus = assimpLoad("models/Venus_1K.obj", true);
	venus.move(glm::vec3(-5, 0, 15));
	venus.grow(glm::vec3(1.3, 1.3, 1.3));
	//sun.addChild(std::move(venus));
	
	auto earth = assimpLoad("models/Earth.obj", true);
	earth.move(glm::vec3(0, 0, 30));
	earth.grow(glm::vec3(0.55, 0.55, 0.55));
	//sun.addChild(std::move(earth));
	
	auto moon = assimpLoad("models/Moon.obj", true);
	moon.move(glm::vec3(0, 0, 6));
	moon.grow(glm::vec3(0.6, 0.6, 0.6));
	earth.addChild(std::move(moon));
	
	// Because boat and tiger are local variables, they will be destroyed when this
	// function terminates. To prevent that, we need to move them into a vector, and then
	// move that vector as part of the return value.
	std::vector<Object3D> objects;
	objects.push_back(std::move(sun));
	objects.push_back(std::move(mercury));
	objects.push_back(std::move(venus));
	objects.push_back(std::move(earth));
	
	// We want these animations to referenced the *moved* objects, which are no longer
	// in the variables named "tiger" and "boat". "boat" is now in the "objects" list at
	// index 0, and "tiger" is the index-1 child of the boat.
	Animator animSun;
	animSun.addAnimation(std::make_unique<RotationAnimation>(objects[0], 60, glm::vec3(0, 6.28, 0)));
	
	Animator animMercuryOrbit;
	animMercuryOrbit.addAnimation(std::make_unique<OrbitalAnimation>(objects[1], 45, objects[0].getPosition(), glm::vec3(0, 1, 0)));
	Animator animMercuryRotation;
	animMercuryRotation.addAnimation(std::make_unique<RotationAnimation>(objects[1], 45, glm::vec3(-2.28, 6.28, -2.28)));
	
	Animator animVenusOrbit;
	animVenusOrbit.addAnimation(std::make_unique<OrbitalAnimation>(objects[2], 45, objects[0].getPosition(), glm::vec3(0, 1, 0)));
	Animator animVenusRotation;
	animVenusRotation.addAnimation(std::make_unique<RotationAnimation>(objects[2], 45, glm::vec3(-2.28, 6.28, -2.28)));
	
	Animator animEarthOrbit;
	animEarthOrbit.addAnimation(std::make_unique<OrbitalAnimation>(objects[3], 45, objects[0].getPosition(), glm::vec3(0, 1, 0)));
	Animator animEarthRotation;
	animEarthRotation.addAnimation(std::make_unique<RotationAnimation>(objects[3], 45, glm::vec3(-2.28, 6.28, -2.28)));

	Animator animMoonOrbit;
	animMoonOrbit.addAnimation(std::make_unique<OrbitalAnimation>(objects[3].getChild(1), 45, objects[3].getPosition(), glm::vec3(0, 1, 0)));
	Animator animMoonRotation;
	animMoonRotation.addAnimation(std::make_unique<RotationAnimation>(objects[3].getChild(1), 45, glm::vec3(-1, 6.28, -1)));

	//Animator animEarth;
	//animEarth.addAnimation(std::make_unique<RotationAnimation>(objects[0], 10, glm::vec3(0, 6.28, 0)));
	//Animator animMoonOrbit;
	//animMoonOrbit.addAnimation(std::make_unique<OrbitalAnimation>(objects[0].getChild(1), 30, objects[0].getPosition(), glm::vec3(0, 1, 0)));
	//Animator animMoonRotation;
	//animMoonRotation.addAnimation(std::make_unique<RotationAnimation>(objects[0].getChild(1), 50, glm::vec3(-6.28, 6.28, -6.28)));

	// The Animators will be destroyed when leaving this function, so we move them into
	// a list to be returned.
	std::vector<Animator> animators;
	animators.push_back(std::move(animSun));
	animators.push_back(std::move(animMercuryOrbit));
	animators.push_back(std::move(animMercuryRotation));
	animators.push_back(std::move(animVenusOrbit));
	animators.push_back(std::move(animVenusRotation));
	animators.push_back(std::move(animEarthOrbit));
	animators.push_back(std::move(animEarthRotation));
	animators.push_back(std::move(animMoonOrbit));
	animators.push_back(std::move(animMoonRotation));

	// Transfer ownership of the objects and animators back to the main.
	return Scene {
		textureMapping(),
		std::move(objects),
		std::move(animators)
	};
}

int main() {
	// Initialize the window and OpenGL.
	sf::ContextSettings Settings;
	Settings.depthBits = 24; // Request a 24 bits depth buffer
	Settings.stencilBits = 8;  // Request a 8 bits stencil buffer
	Settings.antialiasingLevel = 2;  // Request 2 levels of antialiasing
	sf::RenderWindow window(sf::VideoMode{ 1920, 1080 }, "Our Solar System", sf::Style::Resize | sf::Style::Close, Settings);
	gladLoadGL();
	glEnable(GL_DEPTH_TEST);

	// Initialize scene objects.
	auto scene = lifeOfPi();
	// In case you want to manipulate the scene objects directly by name.
	auto& boat = scene.objects[0];
	auto& tiger = boat.getChild(1);

	glm::vec3 cameraPosition = glm::vec3(0, 100, 0);
	glm::vec3 cameraTarget = glm::vec3(0, 0, -1);
	glm::vec3 cameraUp = glm::vec3(0, 1, 0);
	float cameraSpeed = 1.5f;
	float cameraPanSpeed = 0.5f;
	float zoomSpeed = 0.5f;

	auto camera = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
	auto perspective = glm::perspective(glm::radians(45.0), static_cast<double>(window.getSize().x) / window.getSize().y, 0.1, 100.0);

	ShaderProgram& mainShader = scene.defaultShader;
	mainShader.activate();
	mainShader.setUniform("view", camera);
	mainShader.setUniform("projection", perspective);

	// Ready, set, go!
	for (auto& animator : scene.animators) {
		animator.start();
	}
	bool running = true;
	sf::Clock c;

	auto last = c.getElapsedTime();
	while (running) {
		sf::Event ev;
		while (window.pollEvent(ev)) {
			if (ev.type == sf::Event::Closed) {
				running = false;
			}
		}

		// Calculate the elapsed time since the last frame
		auto now = c.getElapsedTime();
		auto diff = now - last;
		float deltaTime = diff.asSeconds();
		last = now;

		// Handle real-time input for camera control
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			cameraPosition.x -= cameraSpeed * deltaTime;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			cameraPosition.x += cameraSpeed * deltaTime;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			cameraPosition.y += cameraSpeed * deltaTime;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			cameraPosition.y -= cameraSpeed * deltaTime;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
			cameraTarget.x -= cameraPanSpeed * deltaTime;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
			cameraTarget.x += cameraPanSpeed * deltaTime;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
			cameraPosition.z -= zoomSpeed * deltaTime;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
			cameraPosition.z += zoomSpeed * deltaTime;
		}

		// Update the camera view matrix
		camera = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
		mainShader.setUniform("view", camera);

		for (auto& animator : scene.animators) {
			animator.tick(deltaTime);
		}

		// Clear the OpenGL "context".
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Render each object in the scene.
		for (auto& o : scene.objects) {
			o.render(window, mainShader);
		}
		window.display();
	}

	return 0;
}



