within DelaunayTables.Types;

class ExternalDelaunayTable

  extends ExternalObject;

  function constructor
    extends Modelica.Icons.Function;
    input Integer nin;
    input Integer nout;
    input Real[:,nin+nout] table;
    input Types.Verbosity verbosity;
    output ExternalDelaunayTable self;

  external "C" self = ExternalDelaunayTable__constructor(
    size(table, 1),
    nin,
    nout,
    table,
    verbosity
  ) annotation (
    IncludeDirectory = "modelica://DelaunayTables/Resources/C-Sources",
    Include = "#include \"DelaunayTable.External.inc\""
   );
  end constructor;

  function destructor
    extends Modelica.Icons.Function;
    input ExternalDelaunayTable self;

  external "C" ExternalDelaunayTable__destructor(
    self
  ) annotation (
    IncludeDirectory = "modelica://DelaunayTables/Resources/C-Sources",
    Include = "#include \"DelaunayTable.External.inc\""
  );
  end destructor;

end ExternalDelaunayTable;
