#include <complexrenderable.hpp>
#include <renderutils.hpp>
#include <shaders.hpp>

// tail-recursive to prevent stack overflow
glm::mat4 ComplexRenderable::hwtHelper(const glm::mat4 &m) const
{
	glm::mat4 mn = getWorldTransform() * m;
	return parent != nullptr ? parent->hwtHelper(mn) : mn;
}

ComplexRenderable::ComplexRenderable() : position(0.0f), angles(0.0f), scales(1.0f), parent(nullptr), texture(0)
{}

glm::mat4 ComplexRenderable::getTranslation() const
{
	return glm::translate(glm::mat4(1.0f), position);
}

glm::mat4 ComplexRenderable::getRotation() const
{
	glm::mat4 rx = glm::rotate(glm::mat4(1.0f), angles.x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 ry = glm::rotate(glm::mat4(1.0f), angles.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rz = glm::rotate(glm::mat4(1.0f), angles.z, glm::vec3(0.0f, 0.0f, 1.0f));
	return rz * ry * rx;
}

glm::mat4 ComplexRenderable::getScaling() const
{
	return glm::scale(glm::mat4(1.0f), scales);
}

const glm::vec3 &ComplexRenderable::getPosition() const
{
	return position;
}

void ComplexRenderable::setPosition(const glm::vec3 &position)
{
	ComplexRenderable::position = position;
}

const glm::vec3 &ComplexRenderable::getAngles() const
{
	return angles;
}

void ComplexRenderable::setAngles(const glm::vec3 &angles)
{
	ComplexRenderable::angles = angles;
}

const glm::vec3 &ComplexRenderable::getScales() const
{
	return scales;
}

void ComplexRenderable::setScales(const glm::vec3 &scales)
{
	ComplexRenderable::scales = scales;
}

ComplexRenderable *ComplexRenderable::getParent() const
{
	return parent;
}

void ComplexRenderable::setParent(ComplexRenderable *parent)
{
	ComplexRenderable::parent = parent;
}

glm::mat4 ComplexRenderable::getWorldTransform() const
{
	return getTranslation() * getRotation() * getScaling();
}

glm::mat4 ComplexRenderable::getHierarchicalWorldTransform() const
{
	return hwtHelper(glm::mat4(1.0f));
}

GLuint ComplexRenderable::getTexture() const
{
	return texture;
}

void ComplexRenderable::setTexture(GLuint texture)
{
	ComplexRenderable::texture = texture;
	textured = true;
}

bool ComplexRenderable::isTextured() const
{
	return textured;
}

ComplexRenderable::~ComplexRenderable() = default;

SimpleComplexRenderable::SimpleComplexRenderable(Renderable *renderable) : renderable(renderable)
{}

SimpleComplexRenderable::~SimpleComplexRenderable()
{
	delete renderable;
}

void SimpleComplexRenderable::render(GLuint shader)
{
	setUniformMat4(shader, MODEL, getHierarchicalWorldTransform());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, getTexture());
	setUniformInt(shader, TEXTURE_MAP, 0);
	setUniformInt(shader, TEXTURE_PRESENT, isTextured());
	renderable->render();
}

Skateboard::Skateboard(ComplexRenderable *character) : character(character)
{
	plank = new SimpleComplexRenderable(createCuboid(glm::vec3(-4.0f, 1.0f, -2.0f), glm::vec3(8.0f, 0.2f, 4.0f)));
	plank->setParent(this);
	for (int i = 0; i < 4; i++)
	{
		wheels[i] = new SimpleComplexRenderable(createCuboid(glm::vec3(-0.5f), glm::vec3(1.0f)));
		wheels[i]->setParent(plank);
	}
	wheels[0]->setPosition(wheels[0]->getPosition() + glm::vec3(-1.5f, 0.5f, -1.5f));
	wheels[1]->setPosition(wheels[1]->getPosition() + glm::vec3(-1.5f, 0.5f, 1.5f));
	wheels[2]->setPosition(wheels[2]->getPosition() + glm::vec3(1.5f, 0.5f, -1.5f));
	wheels[3]->setPosition(wheels[3]->getPosition() + glm::vec3(1.5f, 0.5f, 1.5f));
	character->setParent(this);
}

Skateboard::~Skateboard()
{
	delete plank;
	for (auto &wheel : wheels)
		delete wheel;
	delete character;
}

void Skateboard::render(GLuint shader)
{
	setUniformVec3(shader, OBJECT_COLOR, glm::vec3(193.0f/255.0f, 154.0f/255.0f, 107.0f/255.0f));
	plank->render(shader);
	setUniformVec3(shader, OBJECT_COLOR, glm::vec3(0.0f));
	for (auto &wheel : wheels)
		wheel->render(shader);
	if (character != nullptr)
		character->render(shader);
}

CharB::CharB()
{
	bottom = new SimpleComplexRenderable(createCuboid(glm::vec3(-2.0f, 1.2f, -0.1f), glm::vec3(4.0f, 0.5f, 0.2f)));
	bottom->setParent(this);
	left = new SimpleComplexRenderable(createCuboid(glm::vec3(-2.0f, 1.2f, -0.1f), glm::vec3(0.5f, 5.0f, 0.2f)));
	left->setParent(this);
	bottomRight = new SimpleComplexRenderable(createCuboid(glm::vec3(1.5f, 1.2f, -0.1f), glm::vec3(0.5f, 2.0f, 0.2f)));
	bottomRight->setParent(this);
	topRight = new SimpleComplexRenderable(createCuboid(glm::vec3(1.5f, 4.2f, -0.1f), glm::vec3(0.5f, 2.0f, 0.2f)));
	topRight->setParent(this);
	top = new SimpleComplexRenderable(createCuboid(glm::vec3(-2.0f, 6.2f, -0.1f), glm::vec3(4.0f, 0.5f, 0.2f)));
	top->setParent(this);
	middle = new SimpleComplexRenderable(createCuboid(glm::vec3(-2.0f, 3.2f, -0.1f), glm::vec3(3.5f, 1.0f, 0.2f)));
	middle->setParent(this);
}

CharB::~CharB()
{
	delete top;
	delete left;
	delete topRight;
	delete bottomRight;
	delete bottom;
	delete middle;
}

void CharB::render(GLuint shader)
{
	setUniformVec3(shader, OBJECT_COLOR, glm::vec3(0.0f));
	setUniformInt(shader, TEXTURE_PRESENT, 0);
	bottom->render(shader);
	left->render(shader);
	bottomRight->render(shader);
	topRight->render(shader);
	top->render(shader);
	middle->render(shader);
}

CharO::CharO()
{
	bottom = new SimpleComplexRenderable(createCuboid(glm::vec3(-2.0f, 1.2f, -0.1f), glm::vec3(4.0f, 0.5f, 0.2f)));
	bottom->setParent(this);
	left = new SimpleComplexRenderable(createCuboid(glm::vec3(-2.0f, 1.2f, -0.1f), glm::vec3(0.5f, 5.0f, 0.2f)));
	left->setParent(this);
	right = new SimpleComplexRenderable(createCuboid(glm::vec3(1.5f, 1.2f, -0.1f), glm::vec3(0.5f, 5.0f, 0.2f)));
	right->setParent(this);
	top = new SimpleComplexRenderable(createCuboid(glm::vec3(-2.0f, 6.2f, -0.1f), glm::vec3(4.0f, 0.5f, 0.2f)));
	top->setParent(this);
}

CharO::~CharO()
{
	delete top;
	delete left;
	delete right;
	delete bottom;
}

void CharO::render(GLuint shader)
{
	setUniformVec3(shader, OBJECT_COLOR, glm::vec3(1.0f / 6.0f));
	setUniformInt(shader, TEXTURE_PRESENT, 0);
	bottom->render(shader);
	left->render(shader);
	right->render(shader);
	top->render(shader);
}

CharU::CharU()
{
	bottom = new SimpleComplexRenderable(createCuboid(glm::vec3(-2.0f, 1.2f, -0.1f), glm::vec3(4.0f, 0.5f, 0.2f)));
	bottom->setParent(this);
	left = new SimpleComplexRenderable(createCuboid(glm::vec3(-2.0f, 1.2f, -0.1f), glm::vec3(0.5f, 5.0f, 0.2f)));
	left->setParent(this);
	right = new SimpleComplexRenderable(createCuboid(glm::vec3(1.5f, 1.2f, -0.1f), glm::vec3(0.5f, 5.0f, 0.2f)));
	right->setParent(this);
}

CharU::~CharU()
{
	delete left;
	delete right;
	delete bottom;
}

void CharU::render(GLuint shader)
{
	setUniformVec3(shader, OBJECT_COLOR, glm::vec3(2.0f / 6.0f));
	setUniformInt(shader, TEXTURE_PRESENT, 0);
	bottom->render(shader);
	left->render(shader);
	right->render(shader);
}

CharD::CharD()
{

}

CharD::~CharD()
{
	delete top;
	delete left;
	delete right;
	delete bottom;
}

void CharD::render(GLuint shader)
{
	setUniformVec3(shader, OBJECT_COLOR, glm::vec3(3.0f / 6.0f));
	setUniformInt(shader, TEXTURE_PRESENT, 0);
	bottom->render(shader);
	left->render(shader);
	right->render(shader);
	top->render(shader);
}
