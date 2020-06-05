within DelaunayTables.Tests.Simple;

model I2O1x4
  extends Modelica.Icons.Example;

  import Modelica.Constants.pi;

  Modelica.Blocks.Sources.RealExpression[2] inputData(
    y = sin({11, 13} .* 2 * pi .* time)
  ) annotation (
    Placement(
      visible = true,
      transformation(origin = {-60, 0}, extent = {{-40, -20}, {40, 20}}, rotation = 0)
    )
  );

  function u2y
    extends Modelica.Icons.Function;
    input Real[2] u;
    output Real y;
  algorithm
    y := u[1] * 1.0 + u[2] * 2.0;
  end u2y;

  DelaunayTables.DelaunayTable delaunayTable(
    nin = 2, nout = 1,
    table = [
      -1, -1, u2y({-1, -1});
      -1, +1, u2y({-1, +1});
      +1, -1, u2y({+1, -1});
      +1, +1, u2y({+1, +1})
    ]
  ) annotation (
    Placement(
      visible = true,
      transformation(origin = {30, 30}, extent = {{-10, -10}, {10, 10}}, rotation = 0)
    )
  );

  Real y_result    = delaunayTable.y[1];
  Real y_reference = u2y(inputData.y);

equation

  connect(inputData.y, delaunayTable.u) annotation(
    Line(points = {{-20, 0}, {0, 0}, {0, 30}, {20, 30}}, color = {0, 0, 127}, thickness = 0.5)
  );

end I2O1x4;
