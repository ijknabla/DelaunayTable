within DelaunayTables.Examples;

model TestDelaunayTable
  extends Modelica.Icons.Example;

  DelaunayTables.DelaunayTable delaunayTable annotation(
    Placement(
      visible = true,
      transformation(origin = {-10, 10}, extent = {{-10, -10}, {10, 10}}, rotation = 0))
  );

equation

  delaunayTable.u[:] = zeros(delaunayTable.nin);

end TestDelaunayTable;