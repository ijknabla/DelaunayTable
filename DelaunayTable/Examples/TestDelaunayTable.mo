within DelaunayTable.Examples;

model TestDelaunayTable
  import Modelica.Utilities.Files;

  extends Modelica.Icons.Example;

  DelaunayTable delaunayTable(
    tableName = "tableI2O1",
    fileName = Files.loadResource("modelica://DelaunayTable/Resources/Data/Tables/singlePoint.txt")
  ) annotation(
    Placement(
      visible = true,
      transformation(origin = {-10, 10}, extent = {{-10, -10}, {10, 10}}, rotation = 0))
  );

equation

  delaunayTable.u[:] = zeros(delaunayTable.nin);

end TestDelaunayTable;