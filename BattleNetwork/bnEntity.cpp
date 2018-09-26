#include "bnEntity.h"
#include "bnTile.h"
#include "bnField.h"

Entity::Entity()
  : tile(nullptr),
  next(nullptr),
  previous(nullptr),
  field(nullptr),
  team(Team::UNKNOWN),
  deleted(false),
  passthrough(false),
  ownedByField(false),
  element(Element::NONE) {
}

Entity::~Entity() {
}

void Entity::Update(float _elapsed) {
  if (_elapsed <= 0)
    return;

  if (IsDeleted()) {
    field->RemoveEntity(this);
  }
}

bool Entity::Move(Direction _direction) {
  assert(false && "Move shouldn't be called directly from Entity");
  return false;
}

vector<Drawable*> Entity::GetMiscComponents() {
  assert(false && "GetMiscComponents shouldn't be called directly from Entity");
  return vector<Drawable*>();
}

const float Entity::GetHitHeight() const {
  //assert(false && "GetHitHeight shouldn't be called directly from Entity");
  return 0;
}

const bool Entity::Hit(int damage) {
  //assert(false && "Hit shouldn't be called directly from Entity");
  return false;
}

TextureType Entity::GetTextureType() {
  assert(false && "GetGetTextureType shouldn't be called directly from Entity");
  return (TextureType)0;
}

bool Entity::Teammate(Team _team) {
  return team == _team;
}

void Entity::SetTile(Battle::Tile* _tile) {
  tile = _tile;
}

Battle::Tile* Entity::GetTile() const {
  return tile;
}

void Entity::SetField(Field* _field) {
  field = _field;
}

Field* Entity::GetField() const {
  return field;
}

Team Entity::GetTeam() const {
  return team;
}
void Entity::SetTeam(Team _team) {
  team = _team;
}

void Entity::SetPassthrough(bool state)
{
  passthrough = state;
}

bool Entity::IsPassthrough()
{
  return passthrough;
}

void Entity::SetFloatShoe(bool state)
{
  floatShoe = state;
}

bool Entity::HasFloatShoe()
{
  return floatShoe;
}

void Entity::Delete()
{
  deleted = true;
}

bool Entity::IsDeleted() const {
  return deleted;
}

void Entity::SetElement(Element _elem)
{
  this->element = _elem;
}

const Element Entity::GetElement() const
{
  return element;
}
