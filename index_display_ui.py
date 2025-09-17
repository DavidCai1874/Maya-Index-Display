import maya.cmds as cmds

def create_or_get_overlay_node():
    sel = cmds.ls(selection=True, dag=True, type="mesh")
    if not sel:
        cmds.warning("Pick a mesh to create or get an overlay node.")
        return None, None

    mesh = sel[0]
    transform = cmds.listRelatives(mesh, parent=True, fullPath=True)[0]

    existing = cmds.listConnections(mesh + ".outMesh", type="FaceIndexOverlayNode")
    if existing:
        return mesh, existing[0]

    # create overlayNode and connect
    overlay = cmds.createNode("FaceIndexOverlayNode", name=transform + "_overlayNode")
    cmds.connectAttr(mesh + ".outMesh", overlay + ".inputMesh", force=True)

    overlayTransform = cmds.listRelatives(overlay, parent=True, fullPath=True)[0]

    for attr in ["translate", "rotate", "scale"]:
        for axis in ["X", "Y", "Z"]:
            try:
                cmds.connectAttr(f"{transform}.{attr}{axis}", f"{overlayTransform}.{attr}{axis}", force=True)
            except Exception as e:
                cmds.warning(f" {attr}{axis} failed to connect: {e}")
    
    
    return mesh, overlay

def update_controls_state(*args):
    any_display_on = cmds.checkBox("showFaceCB", q=True, v=True) or \
                     cmds.checkBox("showEdgeCB", q=True, v=True) or \
                     cmds.checkBox("showVertexCB", q=True, v=True)

    cmds.checkBox("cullBackCB", e=True, en=any_display_on)
    cmds.checkBox("highlightCB", e=True, en=any_display_on)
    cmds.intSliderGrp("facingAngleSlider", e=True, en=any_display_on and cmds.checkBox("cullBackCB", q=True, v=True))
    cmds.intSliderGrp("textSizeSlider", e=True, en=any_display_on)

def apply_attribute(attr, value):
    mesh, node = create_or_get_overlay_node()
    if node:
        cmds.setAttr(f"{node}.{attr}", value)
        cmds.select(mesh, r=True)
        update_controls_state()

def on_toggle_checkbox(attr, checkbox):
    def callback(*args):
        value = cmds.checkBox(checkbox, q=True, v=True)
        apply_attribute(attr, value)
    return callback

def on_slider_changed(attr, slider):
    def callback(value):
        apply_attribute(attr, value)
    return callback

def build_ui():
    if cmds.workspaceControl("FaceIndexOverlayWin", exists=True):
            cmds.deleteUI("FaceIndexOverlayWin", control=True)
        
    cmds.workspaceControl("FaceIndexOverlayWin", label="Face Index Tools", retain=False)
    cmds.columnLayout(adjustableColumn=True, rowSpacing=10)

    cmds.text(label="Display Options")

    cmds.checkBox("showFaceCB", label="Show Face", cc=on_toggle_checkbox("showFace", "showFaceCB"))
    cmds.checkBox("showEdgeCB", label="Show Edge", cc=on_toggle_checkbox("showEdge", "showEdgeCB"))
    cmds.checkBox("showVertexCB", label="Show Vertex", cc=on_toggle_checkbox("showVertex", "showVertexCB"))

    cmds.separator(h=10, style="in")

    cmds.checkBox("cullBackCB", label="Cull Back", en=False, cc=on_toggle_checkbox("cullBack", "cullBackCB"))

    cmds.intSliderGrp("facingAngleSlider", label="Facing Angle", field=True, min=30, max=90, value=45, en=False,columnAlign=(1, "left"), 
                      cc=on_slider_changed("facingAngle", "facingAngleSlider"), 
                      dragCommand=lambda value: apply_attribute("facingAngle", value))

    cmds.intSliderGrp("textSizeSlider", label="Text Size", field=True, min=1, max=30, value=10, en=False,columnAlign=(1, "left"), 
                      cc=on_slider_changed("textSize", "textSizeSlider"))

    cmds.checkBox("highlightCB", label="Highlight Selection", en=False, cc=on_toggle_checkbox("highlightSelected", "highlightCB"))

    cmds.setParent("..")
# Call to open the UI
build_ui()

