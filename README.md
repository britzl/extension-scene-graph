# extension-scene-graph
Extension to retrieve the Defold scene graph data.

```lua
local g = graph.dump()
pprint(g)

for _,node in pairs(g.children) do
   print(node.id)
   print(node.position)
   print(node.scale)
   ...
end
```
