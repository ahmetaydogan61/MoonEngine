<h2><b>Setup</b></h2>
<ol>
  <li>Download the repository.</li>
  <li>Run the GenerateSolution.bat file.</li>
  <li>Open MoonEngine.sln and build the solution.</li>
</ol>

Note that you can create a new solution and use MoonEngineCore features like Scene, Entity System, Renderer etc. Make sure that you link MoonEngineCore to the solution and add the necessary include directories.

<h2><b>Features</b></h2>
<ul>
   <li>Batch Renderer</li>
   <li>Entity Component System</li>
   <li>Scene Serializer (Save/Load)</li>
   <li>Particle System</li>
</ul>

<h2><b>The Editor</b></h2>
<ul>
  <li>The editor can be used to create/delete entities, add/remove components, play/stop and save/load scenes.</li>
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
               Rename Scene: Enter a name to the text input at the top right corner then
               press enter.
            </li>
            <li>
               From File you can choose to save/load scenes.
               </br> Saving: Choose a folder and save. It will be saved by the scene name to the chosen directory.
               </br> Loading: Choose a .moon file and load.
               You can drag and drop a scene file from the content browser into the viewport to load a scene.
            </li>
         </ul>
   <h4><b>Content Browser</b></h4> 
   MoonEngine currently doesn't have a project system. Because of that, all the assets are in a constant directory.
         <ul> 
            <li>
               Content Browser Directory: "MoonEngine/MoonEngine/res/Assets"
            </li>
            <li>
              Search file is currently not working.
            </li>
         </ul>
</ul>

<h3><b>Shortcuts</b></h3>
<ul>
  <li>LeftCtrl + D: Duplicate selected entity.</li>
  <li>LeftCtrl + W: Delete selected entity.</li>
  <li>
  For Gizmo operation selection, while not in Play mode;
  </br> Q: Select, W: Translate, E: Scale, R: Rotate.
  </li>
</ul>