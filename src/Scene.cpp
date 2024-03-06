#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
	for (auto it = m_systems.begin(); it != m_systems.end(); it++) {
		(*it)->OnEnd(m_registry);
	}
	m_systems.clear();
}

entt::registry* Scene::GetRegistry()
{
	return &m_registry;
}

int Scene::CreateEntity()
{
	return (int)m_registry.create();
}

int Scene::GetEntityCount()
{
	return m_registry.alive();
}

bool Scene::IsEntity(int entity)
{
	return m_registry.valid((entt::entity) entity);
}

void Scene::RemoveEntity(int entity)
{
	m_registry.destroy((entt::entity)entity);
}

void Scene::lua_openscene(lua_State* L, Scene* scene)
{
	lua_newtable(L);

	luaL_Reg methods[] = {
		// { " MethodNameInLua " , FunctionNameInC }
		{ "GetEntityCount", lua_GetEntityCount },
		{ "CreateEntity", lua_CreateEntity } ,
		{ "IsEntity", lua_IsEntity },
		{ "RemoveEntity", lua_RemoveEntity },
		{ "HasComponent", lua_HasComponent },
		{ "GetComponent", lua_GetComponent },
		{ "SetComponent", lua_SetComponent } ,
		{ "RemoveComponent", lua_RemoveComponent },
		{ "DrawText", lua_DrawText},
		{ "DrawRectangle", lua_DrawRectangle},
		{ "DrawOutline", lua_DrawOutline},
		{ "DamageEnemy", lua_DamageEnemy },
		{ "GetEnemies", lua_GetEnemies },
		{ NULL , NULL } // Has to end with { NULL, NULL }
	};
	
	lua_pushlightuserdata(L, scene);

	luaL_setfuncs(L, methods, 1); // 1 : one upvalue ( lightuserdata )
	
	lua_setglobal(L, "scene");
}

void Scene::UpdateSystems(GameScreen& currentState, float delta)
{
	for (auto it = m_systems.begin(); it != m_systems.end(); it++) {
		if ((*it)->OnUpdate(m_registry, currentState, delta)) {
			delete (*it);
			it = m_systems.erase(it);
		}
	}
}

void Scene::DrawSystems(GameScreen currentState)
{
	for (auto it = m_systems.begin(); it != m_systems.end(); it++) {
		(*it)->Draw(m_registry, currentState);
	}
}

int Scene::RefAndPushBehaviour(lua_State* L, int entity, const char* path)
{
	// Returns behaviour table on top of the stack
	luaL_dofile(L, path);

	// luaL_ref pops value of stack, so we push table again before luaL_ref
	lua_pushvalue(L, -1);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);

	// Populate behaviour table with info it should have
	lua_pushinteger(L, entity);
	lua_setfield(L, -2, "ID");

	lua_pushinteger(L, 1);
	lua_setfield(L, -2, "HP");

	// behaviour creates itself
	lua_getfield(L, -1, "OnCreate");
	lua_pushvalue(L, -2); // Push the table as argument
	lua_pcall(L, 1, 0, 0); // OnCreate()

	return ref;
}

Scene* Scene::lua_GetSceneUpValue(lua_State* L)
{
	Scene* scene = nullptr;
	if (lua_isuserdata(L, lua_upvalueindex(1)))
	{
		scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	}
	return scene;
}

int Scene::lua_GetEntityCount(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int count = scene->GetEntityCount();
	lua_pushinteger(L, count);
	return 1;
}

int Scene::lua_CreateEntity(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int entity = scene->CreateEntity();
	lua_pushinteger(L, entity);
	return 1;
}

int Scene::lua_IsEntity(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int entity = lua_tointeger(L, 1);
	bool alive = scene->IsEntity(entity);
	lua_pushboolean(L, alive);
	return 1;
}

int Scene::lua_RemoveEntity(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int entity = lua_tointeger(L, 1);
	scene->RemoveEntity(entity);
	return 0;
}

int Scene::lua_HasComponent(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int entity = lua_tointeger(L, 1);
	std::string type = lua_tostring(L, 2);
	bool hasComponent = false;
	
	if (type == "tower"){
		hasComponent = scene->HasComponents<Tower>(entity);
	}
	else if (type == "enemy") {
		hasComponent = scene->HasComponents<Enemy>(entity);
	}
	else if (type == "projectile") {
		hasComponent = scene->HasComponents<Projectile>(entity);
	}
	else if (type == "renderable") {
		hasComponent = scene->HasComponents<Renderable>(entity);
	}
	else if (type == "behaviour") {
		hasComponent = scene->HasComponents<Behaviour>(entity);
	}
	else if (type == "camera") {
		hasComponent = scene->HasComponents<CameraComponent>(entity);
	}
	else if (type == "ray") {
		hasComponent = scene->HasComponents<RayComponent>(entity);
	}
	else if (type == "groundtile") {
		hasComponent = scene->HasComponents<GroundTile>(entity);
	}
	else if (type == "pathnode") {
		hasComponent = scene->HasComponents<PathNode>(entity);
	}
	else if (type == "button") {
		hasComponent = scene->HasComponents<Button>(entity);
	}
	else if (type == "statebutton") {
		hasComponent = scene->HasComponents<StateButton>(entity);
	}
	else if (type == "spawnbutton") {
		hasComponent = scene->HasComponents<SpawnButton>(entity);
	}
	else if (type == "text") {
		hasComponent = scene->HasComponents<Text>(entity);
	}
	else if (type == "colordata") {
		hasComponent = scene->HasComponents<ColorData>(entity);
	}
	else if (type == "basebar") {
		hasComponent = scene->HasComponents<BaseBar>(entity);
	}
	else if (type == "position") {
		hasComponent = scene->HasComponents<Position>(entity);
	}

	lua_pushboolean(L, hasComponent);

	return 1;
}

int Scene::lua_GetComponent(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);

	if (!lua_isinteger(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushnil(L);
		return 1;
	}

	int entity = lua_tointeger(L, 1);
	std::string type = lua_tostring(L, 2);

	// Sanity check that the entity exist
	if (!scene->IsEntity(entity))
	{
		lua_pushnil(L);
		return 1;
	}

	if (type == "tower" && scene->HasComponents<Tower>(entity)) {
		Tower& tower = scene->GetComponent<Tower>(entity);
		lua_pushnumber(L, tower.range);
		lua_pushnumber(L, tower.firingRate);
		return 2;
	}
	else if (type == "enemy" && scene->HasComponents<Enemy>(entity)) {
		Enemy& enemy = scene->GetComponent<Enemy>(entity);	
		return 0;
	}
	else if (type == "projectile" && scene->HasComponents<Projectile>(entity)) {
		Projectile& projectile = scene->GetComponent<Projectile>(entity);
		lua_pushvector(L, projectile.position);
		lua_pushvector(L, projectile.startPosition);
		lua_pushvector(L, projectile.transform);
		lua_pushinteger(L, projectile.targetID);
		lua_pushnumber(L, projectile.range);
		return 5;
	}
	else if (type == "renderable" && scene->HasComponents<Renderable>(entity)) {
		Renderable& renderable = scene->GetComponent<Renderable>(entity);
		lua_pushvector(L, renderable.renderBBox.min);
		lua_pushvector(L, renderable.renderBBox.max);
		printf("Got Renderable\n");
		return 2;
	}
	else if (type == "behaviour" && scene->HasComponents<Behaviour>(entity)) {
		Behaviour& behaviour = scene->GetComponent<Behaviour>(entity);
		printf("Got Behaviour\n");
		return 1;
	}
	else if (type == "camera" && scene->HasComponents<CameraComponent>(entity)) {
		CameraComponent& camera = scene->GetComponent<CameraComponent>(entity);
		lua_pushvector(L, camera.camera.position);
		printf("Got Camera\n");
		return 1;
	}
	else if (type == "ray" && scene->HasComponents<RayComponent>(entity)) {
		RayComponent& ray = scene->GetComponent<RayComponent>(entity);
		lua_pushvector(L, ray.ray.position);
		printf("Got Ray\n");
		return 1;
	}
	else if (type == "groundtile" && scene->HasComponents<GroundTile>(entity)) {
		GroundTile& groundtile = scene->GetComponent<GroundTile>(entity);
		lua_pushvector(L, groundtile.topLeft);
		lua_pushvector(L, groundtile.bottomLeft);
		lua_pushvector(L, groundtile.bottomRight);
		lua_pushvector(L, groundtile.topRight);
		lua_pushvector(L, groundtile.center);
		lua_pushnumber(L, groundtile.cellSize);

		return 6;
	}
	else if (type == "pathnode" && scene->HasComponents<PathNode>(entity)) {
		PathNode& pathnode = scene->GetComponent<PathNode>(entity);
		lua_pushinteger(L, pathnode.x);
		lua_pushinteger(L, pathnode.y);
		lua_pushinteger(L, pathnode.gCost);
		lua_pushinteger(L, pathnode.hCost);
		lua_pushinteger(L, pathnode.fCost);
		lua_pushinteger(L, pathnode.prevEnt);

		return 6;
	}
	else if (type == "button" && scene->HasComponents<Button>(entity)) {
		Button& button = scene->GetComponent<Button>(entity);
		Vector4 recData = { button.rec.x, button.rec.y, button.rec.width, button.rec.height };

		lua_pushinteger(L, button.state);
		lua_pushvector4(L, recData);
	
		return 2;
	}
	else if (type == "statebutton" && scene->HasComponents<StateButton>(entity)) {
		StateButton& sbutton = scene->GetComponent<StateButton>(entity);
		lua_pushinteger(L, sbutton.destination);

		return 1;
	}
	else if (type == "spawnbutton" && scene->HasComponents<SpawnButton>(entity)) {
		SpawnButton& spawnbutton = scene->GetComponent<SpawnButton>(entity);
		lua_pushstring(L, spawnbutton.objectName.c_str());

		return 1;
	}
	else if (type == "text" && scene->HasComponents<Text>(entity)) {
		Text& text = scene->GetComponent<Text>(entity);
		lua_pushstring(L, text.text.c_str());
		lua_pushvector(L, text.textData);

		return 2;
	}
	else if (type == "colordata" && scene->HasComponents<ColorData>(entity)) {
		ColorData& color = scene->GetComponent<ColorData>(entity);
		Vector4 colordata = { color.color.r, color.color.g, color.color.b, color.color.a };
		lua_pushvector4(L, colordata);

		return 1;
	}
	else if (type == "basebar" && scene->HasComponents<BaseBar>(entity)) {
		BaseBar& basebar = scene->GetComponent<BaseBar>(entity);
		lua_pushvector4(L, basebar.baseBarData);
		return 1;
	}
	else if (type == "position" && scene->HasComponents<Position>(entity)) {
		Position& pos = scene->GetComponent<Position>(entity);
		lua_pushvector(L, pos.position);
		return 1;
	}

	lua_pushnil(L);
	return 1;
}

int Scene::lua_SetComponent(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int entity = lua_tointeger(L, 1); // first element in lua file function call
	std::string type = lua_tostring(L, 2); // second element in lua file function call
	
	if (type == "tower")
	{
		float range = lua_tonumber(L, 3);
		float firingRate = lua_tonumber(L, 4);

		scene->SetComponent<Tower>(entity, range, firingRate);
	}
	else if(type == "enemy")
	{
		scene->SetComponent<Enemy>(entity, 0, entity);
	}
	else if (type == "projectile")
	{
		Vector3 position = lua_tovector(L, 3);
		Vector3 start = lua_tovector(L, 4);
		Vector3 transform = lua_tovector(L, 5);
		int targetID = lua_tointeger(L, 6);
		float range = lua_tonumber(L, 7);

		scene->SetComponent<Projectile>(entity, position, start, transform, targetID, range);
	}
	else if (type == "renderable")
	{
		std::string modelPath = lua_tostring(L, 3);
		const char* texturePath = lua_tostring(L, 4);
		int scale = lua_tointeger(L, 5);
		Vector4 colorValues = lua_tovector4(L, 6);
		Vector3 pos = lua_tovector(L, 7);
		
		Model renderable = LoadModel(modelPath.c_str());
		Texture2D texture = LoadTexture(texturePath);
		renderable.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
		BoundingBox renderBBox = GetMeshBoundingBox(renderable.meshes[0]);
		Color color = {colorValues.x, colorValues.y , colorValues.z , colorValues.w};
		renderBBox.min = { renderBBox.min.x + pos.x, renderBBox.min.y + pos.y, renderBBox.min.z + pos.z };
		renderBBox.max = { renderBBox.max.x + pos.x, renderBBox.max.y + pos.y, renderBBox.max.z + pos.z };

		scene->SetComponent<Renderable>(entity, renderable, texture, renderBBox, scale, color, false);
	}
	else if (type == "behaviour")
	{
		if (scene->HasComponents<Behaviour>(entity))
		{
			scene->RemoveComponent<Behaviour>(entity);
		}
		
		const char* path = lua_tostring(L, 3);

		int ref = scene->RefAndPushBehaviour(L, entity, path);

		scene->SetComponent<Behaviour>(entity, path, ref);

		return 1;
	}
	else if (type == "camera")
	{
		Vector3 position = lua_tovector(L, 3);
		Vector3 target = lua_tovector(L, 4);
		Vector3 up = lua_tovector(L, 5);
		float fov = lua_tonumber(L, 6);
		int projection = lua_tonumber(L, 7); // 0 = PERSPECTIVE, 1 = ORTHOGRAPHIC
		int mode = lua_tonumber(L, 8); // 0 = CUSTOM, 1 = FREE, 2 = ORBITAL, 3 = FIRST_PERSON, 4 = THIRD_PERSON

		Camera camera = { 0 };
		camera.position = position;
		camera.target = target;
		camera.up = up;
		camera.fovy = fov;
		camera.projection = projection;

		SetCameraMode(camera, mode);

		scene->SetComponent<CameraComponent>(entity, camera, mode);
		return 1;
	}
	else if (type == "ray")
	{
		Ray ray = { lua_tonumber(L, 3) };
		RayCollision collider = {lua_tonumber(L, 4)};
		std::string strObjectName = lua_tostring(L, 5);
		Vector4 colorValues = lua_tovector4(L, 6);
		Color color = {colorValues.x, colorValues.y, colorValues.z, colorValues.w };

		scene->SetComponent<RayComponent>(entity, ray, collider, strObjectName, color);
		return 1;
	}
	else if (type == "groundtile")
	{
		Vector3 topLeft = lua_tovector(L, 3);
		Vector3 bottomLeft = lua_tovector(L, 4);
		Vector3 bottomRight = lua_tovector(L, 5);
		Vector3 topRight = lua_tovector(L, 6);
		Vector3 center = lua_tovector(L, 7);
		float cellSize = (float)lua_tonumber(L, 8);
		Vector4 colorValues = lua_tovector4(L, 9);
		Color color = { colorValues.x, colorValues.y, colorValues.z, colorValues.w };

		scene->SetComponent<GroundTile>(entity, topLeft, bottomLeft, bottomRight, topRight, center, color, cellSize, false, -1, entity);
		return 1;
	}
	else if (type == "pathnode")
	{
		int x = lua_tointeger(L, 3);
		int y = lua_tointeger(L, 4);
		int gCost = lua_tointeger(L, 5);
		int hCost = lua_tointeger(L, 6);
		int fCost = lua_tointeger(L, 7);
		int prevEnt = lua_tointeger(L, 8);
		int blocking = lua_tointeger(L, 9);

		scene->SetComponent<PathNode>(entity, x, y, gCost, hCost, fCost, prevEnt, blocking);
		return 1;
	}
	else if (type == "button"){
		int state = lua_tonumber(L, 3);
		Vector4 recData = lua_tovector4(L, 4);
		Rectangle rectangle;
		rectangle.x = recData.x;
		rectangle.y = recData.y;
		rectangle.width = recData.z;
		rectangle.height = recData.w;

		scene->SetComponent<Button>(entity, state, rectangle);
		return 1;
	}
	else if (type == "statebutton") {
		int destination = lua_tointeger(L, 3);

		scene->SetComponent<StateButton>(entity, destination);
		return 1;
	}
	else if (type == "spawnbutton") {
		std::string objectName = lua_tostring(L, 3);
		scene->SetComponent<SpawnButton>(entity, objectName);
	return 1;
	}
	else if (type == "text")
	{
		std::string text = lua_tostring(L, 3);
		Vector3 textData = lua_tovector(L, 4);

		scene->SetComponent<Text>(entity, text, textData);
		return 1;
	}
	else if (type == "colordata")
	{
		Vector4 colorValues = lua_tovector4(L, 3);
		Color color = { colorValues.x, colorValues.y, colorValues.z, colorValues.w };
		scene->SetComponent<ColorData>(entity, color);
		return 1;
	}
	else if (type == "basebar")
	{
	Vector4 baseBarData = lua_tovector4(L, 3);
	scene->SetComponent<BaseBar>(entity, baseBarData);
	return 1;
	}
	else if (type == "position")
	{
		Vector3 position = lua_tovector(L, 3);
		scene->SetComponent<Position>(entity, position);
		return 1;
	}

	return 0;
}

int Scene::lua_RemoveComponent(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);

	int entity = lua_tointeger(L, 1);
	std::string type = lua_tostring(L, 2);

	if (type == "tower" && scene->HasComponents<Tower>(entity))
		scene->RemoveComponent<Tower>(entity);
	else if (type == "enemy" && scene->HasComponents<Enemy>(entity))
		scene->RemoveComponent<Enemy>(entity);
	else if (type == "projectile" && scene->HasComponents<Projectile>(entity))
		scene->RemoveComponent<Projectile>(entity);
	else if (type == "renderable" && scene->HasComponents<Renderable>(entity)) {
		UnloadTexture(scene->GetComponent<Renderable>(entity).renderableTexture);
		UnloadModel(scene->GetComponent<Renderable>(entity).renderableModel);
		scene->RemoveComponent<Renderable>(entity);
	}
	else if (type == "behaviour" && scene->HasComponents<Behaviour>(entity))
		scene->RemoveComponent<Behaviour>(entity);
	else if (type == "camera" && scene->HasComponents<CameraComponent>(entity))
		scene->RemoveComponent<CameraComponent>(entity);
	else if (type == "ray" && scene->HasComponents<RayComponent>(entity))
		scene->RemoveComponent<RayComponent>(entity);
	else if (type == "groundtile" && scene->HasComponents<GroundTile>(entity))
		scene->RemoveComponent<GroundTile>(entity);
	else if (type == "pathnode" && scene->HasComponents<PathNode>(entity))
		scene->RemoveComponent<PathNode>(entity);
	else if (type == "button" && scene->HasComponents<Button>(entity))
		scene->RemoveComponent<Button>(entity);
	else if (type == "statebutton" && scene->HasComponents<StateButton>(entity))
		scene->RemoveComponent<StateButton>(entity);
	else if (type == "spawnbutton" && scene->HasComponents<SpawnButton>(entity))
		scene->RemoveComponent<SpawnButton>(entity);
	else if (type == "text" && scene->HasComponents<Text>(entity))
		scene->RemoveComponent<Text>(entity);
	else if (type == "colordata" && scene->HasComponents<ColorData>(entity))
		scene->RemoveComponent<ColorData>(entity);
	else if (type == "position" && scene->HasComponents<Position>(entity))
		scene->RemoveComponent<Position>(entity);
	return 0;
}

int Scene::lua_DrawText(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	std::string text = lua_tostring(L, 1);
	int posX = lua_tonumber(L, 2);
	int posY = lua_tonumber(L, 3);
	int fontSize = lua_tonumber(L, 4);
	Vector4 colorData = lua_tovector4(L, 5);
	Color color = { colorData.x, colorData.y, colorData.z, colorData.w };

	DrawText(text.c_str(), posX, posY, fontSize, color);

	return 0;
}

int Scene::lua_DrawRectangle(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int posX = lua_tonumber(L, 1);
	int posY = lua_tonumber(L, 2);
	int screenWidth = lua_tonumber(L, 3);
	int screenHeight = lua_tonumber(L, 4);
	Vector4 colorData = lua_tovector4(L, 5);
	Color color = { colorData.x, colorData.y, colorData.z, colorData.w };
	DrawRectangle(posX, posY, screenWidth, screenHeight, color);
	return 0;
}

int Scene::lua_DrawOutline(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	Vector4 data = lua_tovector4(L, 1);
	Vector4 colorData = lua_tovector4(L, 2);
	Color color = { colorData.x, colorData.y, colorData.z, colorData.w };

	DrawRectangleLines(data.x, data.y, data.z, data.w, color);
	return 0;
}

int Scene::lua_DamageEnemy(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int entity = lua_tointeger(L, 1);
	auto view = scene->m_registry.view<Enemy, Behaviour>();
	view.each([&](Enemy& enemy, Behaviour& script) {
		if (entity == enemy.entity) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
			lua_getfield(L, -1, "TakeDamage");
			lua_pushvalue(L, -2);
			if (lua_pcall(L, 1, 0, 0) != LUA_OK)
				DumpError(L);
			lua_pop(L, 1);
		}
	});

	return 0;
}

int Scene::lua_GetEnemies(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);

	int i = 0;
	auto view = scene->m_registry.view<Enemy>();
	lua_newtable(L);
	int newTable = lua_gettop(L);
	view.each([&](Enemy& enemy) {
		lua_pushinteger(L, enemy.entity);
		lua_rawseti(L, newTable, i + 1);
		i++;
	});


	//DumpStack(L);

	return 1;
}

