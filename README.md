# Wireframe

This DLL mod re-implements the `/polygonmode` chat command from 0.1.11 Scrap Mechanic and adds a couple physics debug visualization features as well.

## Showcase
Pictures say more than a thousand words, so take a look:

#### First, `/polygonmode`:
<img width="1272" height="716" alt="polygonmode1" src="https://github.com/user-attachments/assets/279d3cf0-bc75-4e19-9caf-bbbfce9e14f9" />
<img width="1270" height="718" alt="polygonmode2" src="https://github.com/user-attachments/assets/8d6791c9-23cf-498e-929e-a91c491a54f7" />

#### Now, physics visualization, such as showing accurate wireframes of the collision mesh...
<img width="1278" height="719" alt="collision" src="https://github.com/user-attachments/assets/1e651c1c-2210-4f93-b618-d828c82d457b" />

#### ...Including AreaTrigger hitboxes!
<img width="828" height="616" alt="areatrigger" src="https://github.com/user-attachments/assets/dab6dd50-dd64-439f-baf2-8dc8c3d2ec67" />


#### AABB visualization ...
<img width="1010" height="714" alt="aabb" src="https://github.com/user-attachments/assets/a1bd9333-92a3-406c-9c2b-d8bfb982ec41" />

#### ... Collision contact point visualization and contact counts ...
<img width="760" height="624" alt="contacts" src="https://github.com/user-attachments/assets/d8f2d8f9-bc26-44fa-bf8b-7f568886eb62" />
<img width="152" height="79" alt="contactcounts" src="https://github.com/user-attachments/assets/5ef2ce78-a776-42e1-abbf-413cecaf1e35" />

#### ... and last but not least, character capsule visualization:
<img width="913" height="696" alt="capsule" src="https://github.com/user-attachments/assets/edc3067a-915a-483c-b423-002774064b6c" />

### UPDATE 1.2.0: Terrain and static objects visualization!
<img width="1920" height="1080" alt="worldDebug2" src="https://github.com/user-attachments/assets/c1c1a129-ed29-4e00-88b4-3f7494a4ecb5" />
<img width="1920" height="1080" alt="worldDebug" src="https://github.com/user-attachments/assets/3f586805-f5aa-44a3-bf5e-28d04fc1c73a" />

## How to use

Simply inject the DLL using a DLL injector of your choice.

**All features of the mod are controlled through in-game chat commands, which are explained below.**  
All commands also have in-game descriptions available through the `/help` command.  
**Commands prefixed with `wf_phys_` are specific to physics-related features.**

- `/polygonmode`: Switches all rendering between normal and wireframe render mode.
- `/wf_hide`: Toggles whether the below chat commands are shown in the command autocomplete or not.  
This is purely visual to reduce chat pollution, the commands still work even if invisible.
- `/wf_phys_setworld [integer]`: Scrap Mechanic simulates 4 (four!) physics worlds per one in-game world.  
This command selects which of these worlds to render:
  - 0: Renders `TickDynamicsWorld` (default, likely the main world).
  - 1: Renders `TickRaycastWorld` - World used for raycasts in the main tick loop (40 TPS). (?)
  - 2: Renders `InterpolatedRaycastWorld` - World used for raycasts in the render loop, is interpolated to avoid jitter. (?)
  - 3: Renders `PhysicsWorld` - Physics world #4, it seems to be empty, not sure what its purpose is.  
  **Note: The world information above may not be 100% accurate at the time of writing this.**
- `/wf_phys_all`: Toggles all physics-related features on or off at once.
- `/wf_phys_wireframe`: Toggles rendering of physics collision mesh wireframe.
- `/wf_phys_aabb`: Toggles rendering of physics object's AABB (bounding box).
- `/wf_phys_contacts`: Toggles rendering of collision contact points.
- `/wf_phys_constraints`: Toggles rendering of physics constraint(joint) origins.
- `/wf_phys_constraintLimits`: Toggles rendering of physics constraint limits.
- `/wf_phys_normals`: Toggles rendering of collision mesh normals.
- `/wf_phys_transforms`: Toggles rendering of physics object's transforms (axes). **Note: Requires wireframe to be enabled!**
- `/wf_phys_capsules`: Toggles rendering of character capsules. **Note: Requires wireframe to be enabled!**
- `/wf_phys_renderdistance [number]` Sets the physics debug render distance in meters, around the camera.
- `/wf_phys_contactcount`: Shows the number of collision contacts that were registered in the previous frame. **Note: Requires contact rendering to be enabled!**
- `/wf_phys_showHiddenObjects`: Toggles rendering of object data for objects that were marked to not render (e.g. terrain cells).

**Note: When using `/wf_phys_wireframe` together with `/wf_phys_showHiddenObjects`, the game may freeze or lag a bit when moving around the world for the
first couple seconds, as the mod has to generate a large amount of debug data in the background. This data is cached and lag will stop after it is generated.**

## Collision wireframe colors

When collision wireframe rendering is enabled, the wireframe will show different colors.  
Here's their meaning:
- `white`: An active object, e.g. a dynamic body that is moving around.
- `green`: A deactivated(sleeping) object, e.g. a dynamic body that, as a result of not moving, was set to sleep.
- `cyan`: An object that is waiting to be set to sleep, but is not actually sleeping just yet.
- **AABBs** are always **red** and **collision contact points** are always **yellow**.

## Known issues

- `/wf_phys_constraintlimits` and `wf_phys_normals` don't seem to work/render properly.  
Normals are only rendered when available, which currently seems to only be the case for terrain cell meshes (shown with hidden objects enabled).  
Constraint limit rendering seems to be an issue with the game's physics engine implementation. Might be fixed in a future mod update.
- The visualization jitters when moving around.  
This depends on which physics world is being visualized - see `/wf_phys_setworld` (**tick** worlds are not interpolated and will thus jitter).
- Some collision shapes appear to clip into each other a bit at the edges even when no forces are applied to them.  
The visualization is not 100% accurate.
