Moon Engine is a 2D rendering application which currently in a development state.
</br>

<h2><b>Setup</b></h2>
<ol>
  <li>Download the repository.</li>
  <li>Run the MakeProject.bat file.</li>
  <li>Open MoonEngine2D.sln and build the solution.</li>
</ol>

Note that you can create a new solution and use "MoonEngine" features like Scene, Entity System, Renderer etc. Make sure that you link "MoonEngine" to the solution and add the necessary include directories.

<h2><b>Features</b></h2>
<ul>
   <li>Batch Renderer</li>
   <li>Particle System</li>
   <li>Scene Serializer/Deserializer with simple Reflection</li>
   <li>Entity Component System wrapper for entt</li>
   <li>Box2D Physics</li>
</ul>

<h2><b>The Editor</b></h2>
The editor can be used to create/delete entities, add/remove components, play/pause/stop and save/load scenes.
<ul>
   <h4><b>Camera</b></h4>
      <ul>
         <li>
            Movement: LeftShift + W/A/S/D or Middle Mouse Button.
         </li>
         <li>
            Zoom: Mouse wheel to zoom in/out.
         </li> 
      </ul>
   <h4><b>Scene Management</b></h4> 
      <ul> 
         <li>
            From File you can choose to save/load scenes.
         </li>
      </ul>
   <h4><b>Content Browser</b></h4> 
   MoonEngine currently doesn't have a project system. Because of that, all the assets are in a constant directory.
      <ul> 
         <li>
            Content Browser Directory: "MoonEngine/MoonEditor/Resource/Assets"
         </li>
         <li>
           Search file: Search any file inside browser directory.
         </li>
      </ul>
</ul>

<h3><b>Shortcuts</b></h3>
<ul>
  <li>LeftCtrl+D: Duplicate selected entity.</li>
  <li>Delete: Delete selected entity.</li>
  <li>LeftCtrl+N: New Scene.</li>
  <li>LeftCtrl+S: Save Scene.</li>
  <li>LeftCtrl+L: Load Scene.</li>
  <li>
  Gizmo operation selection
  </br> Q: Select, W: Translate, E: Scale, R: Rotate.
  </li>
</ul>
