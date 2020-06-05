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
    y := 0;
    for i in 1:size(u,1) loop
      y := y + u[i] * i;
    end for;
  end u2y;

  block UToY
    extends Modelica.Blocks.Interfaces.MIMO(
      final nin=2, final nout=1
    );
  equation
    y[1] = u2y(u);
  end UToY;

  DelaunayTables.DelaunayTable delaunayTable(
    nin = 2, nout = 1,
    verbosity = DelaunayTables.Types.Verbosity.detail,
    table = [
      -1, -1, u2y({-1, -1});
      -1, +1, u2y({-1, +1});
      +1, -1, u2y({+1, -1});
      +1, +1, u2y({+1, +1})
    ]
  ) annotation (
    Placement(
      visible = true,
      transformation(origin = {30, +30}, extent = {{-10, -10}, {10, 10}}, rotation = 0)
    )
  );

  UToY uToY(
  ) annotation (
    Placement(
      visible = true,
      transformation(origin = {30, -30}, extent = {{-10, -10}, {10, 10}}, rotation = 0)
    )
  );

  Real y_result    = delaunayTable.y[1];
  Real y_reference = uToY.y[1];

equation

  connect(inputData.y, delaunayTable.u) annotation(
    Line(points = {{-20, 0}, {0, 0}, {0, +30}, {20, +30}}, color = {0, 0, 127}, thickness = 0.5)
  );
  connect(inputData.y, uToY.u) annotation(
    Line(points = {{-20, 0}, {0, 0}, {0, -30}, {20, -30}}, color = {0, 0, 127}, thickness = 0.5)
  );

end I2O1x4;
