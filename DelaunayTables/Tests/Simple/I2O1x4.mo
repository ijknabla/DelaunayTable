within DelaunayTables.Tests.Simple;

model I2O1x4
  extends Modelica.Icons.Example;

  import Modelica.Constants.pi;

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
      transformation(origin = {0, 0}, extent = {{-20, -20}, {20, 20}}, rotation = 0)
    )
  );

  function u2y
    extends Modelica.Icons.Function;
    input Real[2] u;
    output Real y;
  algorithm
    y := u[1] * 1.0 + u[2] * 2.0;
  end u2y;

  Real[2] u = sin({11, 13} .* 2*pi .* time);

  Real y_result    = delaunayTable.y[1];
  Real y_reference = u2y(u);

equation

  delaunayTable.u[:] = u[:];

end I2O1x4;