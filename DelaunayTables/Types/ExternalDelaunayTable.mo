within DelaunayTables.Types;

class ExternalDelaunayTable

  extends ExternalObject;

  function constructor
    extends Modelica.Icons.Function;
    output ExternalDelaunayTable externalDelaunayTable;

  external "C" externalDelaunayTable = DelaunayTable__open(
  ) annotation (
    IncludeDirectory = "modelica://DelaunayTables/Resources/C-Sources",
    Include = "#include \"DelaunayTable.External.inc\""
   );
  end constructor;

  function destructor
    extends Modelica.Icons.Function;
    input ExternalDelaunayTable externalDelaunayTable;

  external "C" DelaunayTable__close(
    externalDelaunayTable
  ) annotation (
    IncludeDirectory = "modelica://DelaunayTables/Resources/C-Sources",
    Include = "#include \"DelaunayTable.External.inc\""
  );
  end destructor;

end ExternalDelaunayTable;
