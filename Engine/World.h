#pragma once
class World
{
public:
	World(void);
	~World(void);

//////////////////////////////////////////////////////////////////////////
//
//  World will contain:
//		TerrainManager
//			*	TerrainManager will contain all of the terrain meshes, vegetation, procedural generation and the likes
//			
//		EntityManager
//			*	EntityManager will contain all of the entities, obviously. Handle logic like spawning/death, culling before rendering and that stuff. Or maybe world will do the culling?
//			
//		CollisionManager
//			*	Obvious what it will manage. You send in entities from entitymanager and you send in terrain from terrainmanager, and you check collisions and return results.	
//		
//		PhysicsManager
//			*	Might not even have a physics manager. Could also be responsible for movement and the likes, I guess? Windgeneration maybe?
//		
//		LightManager
//			*	Owns all instances of directional, point and spot lights. When it's time to render, we send a pointer of this manager to the renderer.
//			*	Probably also owns the mesh for point/spot light
//		
//		
//		This class might also contain:
//		Day/Night cycle
//		Skysphere? Maybe it belongs in renderer
//		LOD system/function
//		General culling solutions
//		A queue or some sort of container for all objects that will be rendered this frame
//		
//		
//
//		Maybe this class also tracks events and other sorts of things that might actually appear somewhere in the next years. Gameplay and such.
// 
//		In general, it will contain functions bridging all of the managers together
// 
//////////////////////////////////////////////////////////////////////////

};

