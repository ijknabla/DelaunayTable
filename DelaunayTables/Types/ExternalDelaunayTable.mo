within DelaunayTables.Types;

class ExternalDelaunayTable

  extends ExternalObject;

  function constructor
    extends Modelica.Icons.Function;
    input Integer nin;
    input Integer nout;
    input Real[:,nin+nout] table;
    output ExternalDelaunayTable externalDelaunayTable;

  external "C" externalDelaunayTable = ExternalDelaunayTable__constructor(
    size(table, 1),
    nin,
    nout,
    table
  ) annotation (
    IncludeDirectory = "modelica://DelaunayTables/Resources/C-Sources",
    Include = "#include \"DelaunayTable.External.inc\""
   );
  end constructor;

  function destructor
    extends Modelica.Icons.Function;
    input ExternalDelaunayTable externalDelaunayTable;

  external "C" ExternalDelaunayTable__destructor(
    externalDelaunayTable
  ) annotation (
    IncludeDirectory = "modelica://DelaunayTables/Resources/C-Sources",
    Include = "#include \"DelaunayTable.External.inc\""
  );
  end destructor;

end ExternalDelaunayTable;
