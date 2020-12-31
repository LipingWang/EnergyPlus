// EnergyPlus, Copyright (c) 1996-2020, The Board of Trustees of the University of Illinois,
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy), Oak Ridge
// National Laboratory, managed by UT-Battelle, Alliance for Sustainable Energy, LLC, and other
// contributors. All rights reserved.
//
// NOTICE: This Software was developed under funding from the U.S. Department of Energy and the
// U.S. Government consequently retains certain rights. As such, the U.S. Government has been
// granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable,
// worldwide license in the Software to reproduce, distribute copies to the public, prepare
// derivative works, and perform publicly and display publicly, and to permit others to do so.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// (1) Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//
// (3) Neither the name of the University of California, Lawrence Berkeley National Laboratory,
//     the University of Illinois, U.S. Dept. of Energy nor the names of its contributors may be
//     used to endorse or promote products derived from this software without specific prior
//     written permission.
//
// (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in stand-alone form
//     without changes from the version obtained under this License, or (ii) Licensee makes a
//     reference solely to the software portion of its product, Licensee must refer to the
//     software as "EnergyPlus version X" software, where "X" is the version number Licensee
//     obtained under this License and may not use a different name for the software. Except as
//     specifically required in this Section (4), Licensee shall not use in a company name, a
//     product name, in advertising, publicity, or other promotional activities any name, trade
//     name, trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or confusingly
//     similar designation, without the U.S. Department of Energy's prior written consent.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// C++ Headers
#include <cassert>
#include <cmath>
#include <iomanip>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

// ObjexxFCL Headers
#include <ObjexxFCL/Array.functions.hh>
#include <ObjexxFCL/ArrayS.functions.hh>
#include <ObjexxFCL/Fmath.hh>
#include <ObjexxFCL/member.functions.hh>
#include <ObjexxFCL/numeric.hh>
#include <ObjexxFCL/string.functions.hh>
#include <ObjexxFCL/time.hh>

// Third-party Headers
#include <fmt/format.h>

// EnergyPlus Headers
#include <AirflowNetwork/Elements.hpp>
#include <EnergyPlus/Boilers.hh>
#include <EnergyPlus/ChillerElectricEIR.hh>
#include <EnergyPlus/ChillerReformulatedEIR.hh>
#include <EnergyPlus/CondenserLoopTowers.hh>
#include <EnergyPlus/CostEstimateManager.hh>
#include <EnergyPlus/DXCoils.hh>
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/DataAirLoop.hh>
#include <EnergyPlus/DataDaylighting.hh>
#include <EnergyPlus/DataDefineEquip.hh>
#include <EnergyPlus/DataGlobalConstants.hh>
#include <EnergyPlus/DataHVACGlobals.hh>
#include <EnergyPlus/DataHeatBalFanSys.hh>
#include <EnergyPlus/DataHeatBalSurface.hh>
#include <EnergyPlus/DataHeatBalance.hh>
#include <EnergyPlus/DataIPShortCuts.hh>
#include <EnergyPlus/DataOutputs.hh>
#include <EnergyPlus/DataShadowingCombinations.hh>
#include <EnergyPlus/DataSizing.hh>
#include <EnergyPlus/DataStringGlobals.hh>
#include <EnergyPlus/DataSurfaces.hh>
#include <EnergyPlus/DataWater.hh>
#include <EnergyPlus/DataZoneEquipment.hh>
#include <EnergyPlus/DisplayRoutines.hh>
#include <EnergyPlus/EconomicLifeCycleCost.hh>
#include <EnergyPlus/ElectricPowerServiceManager.hh>
#include <EnergyPlus/EvaporativeCoolers.hh>
#include <EnergyPlus/EvaporativeFluidCoolers.hh>
#include <EnergyPlus/FileSystem.hh>
#include <EnergyPlus/FluidCoolers.hh>
#include <EnergyPlus/General.hh>
#include <EnergyPlus/HVACVariableRefrigerantFlow.hh>
#include <EnergyPlus/HeatingCoils.hh>
#include <EnergyPlus/HybridModel.hh>
#include <EnergyPlus/InputProcessing/InputProcessor.hh>
#include <EnergyPlus/InternalHeatGains.hh>
#include <EnergyPlus/LowTempRadiantSystem.hh>
#include <EnergyPlus/MixedAir.hh>
#include <EnergyPlus/OutputProcessor.hh>
#include <EnergyPlus/OutputReportPredefined.hh>
#include <EnergyPlus/OutputReportTabular.hh>
#include <EnergyPlus/OutputReportTabularAnnual.hh>
#include <EnergyPlus/PackagedThermalStorageCoil.hh>
#include <EnergyPlus/PlantChillers.hh>
#include <EnergyPlus/PollutionModule.hh>
#include <EnergyPlus/Psychrometrics.hh>
#include <EnergyPlus/RefrigeratedCase.hh>
#include <EnergyPlus/ReportCoilSelection.hh>
#include <EnergyPlus/ResultsFramework.hh>
#include <EnergyPlus/SQLiteProcedures.hh>
#include <EnergyPlus/ScheduleManager.hh>
#include <EnergyPlus/ThermalComfort.hh>
#include <EnergyPlus/UtilityRoutines.hh>
#include <EnergyPlus/VentilatedSlab.hh>
#include <EnergyPlus/WaterThermalTanks.hh>
#include <EnergyPlus/WeatherManager.hh>
#include <EnergyPlus/ZonePlenum.hh>
#include <EnergyPlus/ZoneTempPredictorCorrector.hh>

namespace EnergyPlus::OutputReportTabular {

    // MODULE INFORMATION:
    //    AUTHOR         Jason Glazer of GARD Analytics, Inc.
    //    DATE WRITTEN   July 2003
    //    MODIFIED       na
    //    RE-ENGINEERED  na
    // PURPOSE OF THIS MODULE:
    //    This module allows the user to define several different tabular
    //    report that have a specific format.
    // METHODOLOGY EMPLOYED:
    //    Generally aggregation. Specifically, the IDF objects are read into data
    //    structures on the first call to update the data.  The data structures
    //    include not only ones to hold the IDF data but also that initialize
    //    the structure used to gather data each iteration. The report:table:binned
    //    object is stored in OutputTableBinned.
    //    During initialization the TableResults data structure is created which contains
    //    all the information needed to perform the aggregation on a timestep basis.
    //    After the end of the simulation the original Output data structures
    //    are scanned and actual tables are created doing any scaling as necessary
    //    and placing all the results into an output table.  The output table
    //    is written in the selected format for each of the tables defined.
    // REFERENCES:
    //    None.
    // OTHER NOTES:.
    //                                      |--> BinResults
    //                                      |
    //                                      |--> BinResultsAbove
    //   OutputTableBinned ---------------->|
    //                                      |--> BinResultsBelow
    //                                      |
    //                                      |--> BinObjVarID
    //
    //                                      |--> MonthlyFieldSetInput
    //   MonthlyInput --------------------->|
    //                                      |--> MonthlyTable --> MonthlyColumns

    // Using/Aliasing
    using namespace DataGlobalConstants;
    using namespace OutputReportPredefined;
    using namespace DataHeatBalance;
    using namespace HybridModel;

    // Functions
    void clear_state(EnergyPlusData &state)
    {
        OutputReportTabular::ResetTabularReports(state);
    }

    std::ofstream & open_tbl_stream(EnergyPlusData &state, int const iStyle, std::string const & filename, bool output_to_file)
    {
        std::ofstream &tbl_stream(*state.dataOutRptTab->TabularOutputFile(iStyle));
        if (output_to_file) {
            tbl_stream.open(filename);
            if (!tbl_stream) {
                ShowFatalError(state, "OpenOutputTabularFile: Could not open file \"" + filename +
                               "\" for output (write).");
            }
        } else {
            tbl_stream.setstate(std::ios_base::badbit);
        }
        return tbl_stream;
    }

    void UpdateTabularReports(EnergyPlusData &state, OutputProcessor::TimeStepType t_timeStepType) // What kind of data to update (Zone, HVAC)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   July 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // This is the routine that is called at the end of the time step
        // loop and updates the arrays of data that will later being put
        // into the tabular reports.

        auto &ort(state.dataOutRptTab);

        if (t_timeStepType != OutputProcessor::TimeStepType::TimeStepZone && t_timeStepType != OutputProcessor::TimeStepType::TimeStepSystem) {
            ShowFatalError(state, "Invalid reporting requested -- UpdateTabularReports");
        }

        if (ort->UpdateTabularReportsGetInput) {
            GetInputTabularMonthly(state);
            OutputReportTabularAnnual::GetInputTabularAnnual(state);
            OutputReportTabularAnnual::checkAggregationOrderForAnnual(state);
            GetInputTabularTimeBins(state);
            GetInputTabularStyle(state);
            GetInputOutputTableSummaryReports(state);
            // noel -- noticed this was called once and very slow -- sped up a little by caching keys
            InitializeTabularMonthly(state);
            if (isInvalidAggregationOrder(state)) {
                ShowFatalError(state, "OutputReportTabular: Invalid aggregations detected, no simulation performed.");
            }
            GetInputFuelAndPollutionFactors(state);
            SetupUnitConversions(state);
            AddTOCLoadComponentTableSummaries(state);
            ort->UpdateTabularReportsGetInput = false;
            date_and_time(_, _, _, ort->td);
        }
        if (state.dataGlobal->DoOutputReporting && ort->WriteTabularFiles && (state.dataGlobal->KindOfSim == DataGlobalConstants::KindOfSim::RunPeriodWeather)) {
            if (t_timeStepType == OutputProcessor::TimeStepType::TimeStepZone) {
                ort->gatherElapsedTimeBEPS += state.dataGlobal->TimeStepZone;
            }
            if (state.dataGlobal->DoWeathSim) {
                GatherMonthlyResultsForTimestep(state, t_timeStepType);
                OutputReportTabularAnnual::GatherAnnualResultsForTimeStep(state, t_timeStepType);
                GatherBinResultsForTimestep(state, t_timeStepType);
                GatherBEPSResultsForTimestep(state, t_timeStepType);
                GatherSourceEnergyEndUseResultsForTimestep(state, t_timeStepType);
                GatherPeakDemandForTimestep(state, t_timeStepType);
                GatherHeatGainReport(state, t_timeStepType);
                GatherHeatEmissionReport(state, t_timeStepType);
            }
        }
    }

    //======================================================================================================================
    //======================================================================================================================

    //    GET INPUT ROUTINES

    //======================================================================================================================
    //======================================================================================================================

    void GetInputTabularMonthly(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   July 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   The routine assigns the input information for
        //   REPORT:TABLE:MONTHLY also known as tabular monthly
        //   reports that are defined by the user. The input
        //   information is assigned to a data structure that
        //   is used for both user defined monthly reports and
        //   predefined monthly reports.

        // METHODOLOGY EMPLOYED:
        //   Uses get input structure and call to build up
        //   data on monthly reports.

        // REFERENCES:
        // na

        // USE STATEMENTS:
        // na

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        static std::string const CurrentModuleObject("Output:Table:Monthly");

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int curTable;             // index of the current table being processed in MonthlyInput
        iAggType curAggType;           // kind of aggregation identified (see AggType parameters)
        std::string curAggString; // Current aggregation sting
        int jField;
        int NumParams;            // Number of elements combined
        int NumAlphas;            // Number of elements in the alpha array
        int NumNums;              // Number of elements in the numeric array
        Array1D_string AlphArray; // character string data
        Array1D<Real64> NumArray; // numeric data
        int IOStat;               // IO Status when calling get input subroutine
        static bool ErrorsFound(false);
        auto &ort(state.dataOutRptTab);

        if (!(state.files.outputControl.tabular || state.files.outputControl.sqlite)) {
            ort->WriteTabularFiles = false;
            return;
        }

        ort->MonthlyInputCount = inputProcessor->getNumObjectsFound(state, CurrentModuleObject);
        if (ort->MonthlyInputCount > 0) {
            ort->WriteTabularFiles = true;
            // if not a run period using weather do not create reports
            if (!state.dataGlobal->DoWeathSim) {
                ShowWarningError(state, CurrentModuleObject + " requested with SimulationControl Run Simulation for Weather File Run Periods set to No so " +
                                 CurrentModuleObject + " will not be generated");
                return;
            }
        }
        inputProcessor->getObjectDefMaxArgs(state, CurrentModuleObject, NumParams, NumAlphas, NumNums);
        AlphArray.allocate(NumAlphas);
        NumArray.dimension(NumNums, 0.0);
        for (int TabNum = 1, TabNum_end = ort->MonthlyInputCount; TabNum <= TabNum_end; ++TabNum) { // MonthlyInputCount is modified in the loop
            inputProcessor->getObjectItem(state, CurrentModuleObject, TabNum, AlphArray, NumAlphas, NumArray, NumNums, IOStat);

            if (TabNum - 1 > 0) {
                UtilityRoutines::IsNameEmpty(state, AlphArray(1), CurrentModuleObject, ErrorsFound);
            }
            if (NumAlphas < 2) {
                ShowSevereError(state, CurrentModuleObject + ": No fields specified.");
            }
            // add to the data structure
            curTable = AddMonthlyReport(state, AlphArray(1), int(NumArray(1)));
            for (jField = 2; jField <= NumAlphas; jField += 2) {
                if (AlphArray(jField).empty()) {
                    ShowFatalError(state, "Blank report name in Output:Table:Monthly");
                }
                curAggString = AlphArray(jField + 1);
                // set accumulator values to default as appropriate for aggregation type
                if (UtilityRoutines::SameString(curAggString, "SumOrAverage")) {
                    curAggType = iAggType::SumOrAvg;
                } else if (UtilityRoutines::SameString(curAggString, "Maximum")) {
                    curAggType = iAggType::Maximum;
                } else if (UtilityRoutines::SameString(curAggString, "Minimum")) {
                    curAggType = iAggType::Minimum;
                } else if (UtilityRoutines::SameString(curAggString, "ValueWhenMaximumOrMinimum")) {
                    curAggType = iAggType::ValueWhenMaxMin;
                } else if (UtilityRoutines::SameString(curAggString, "HoursZero")) {
                    curAggType = iAggType::HoursZero;
                } else if (UtilityRoutines::SameString(curAggString, "HoursNonzero")) {
                    curAggType = iAggType::HoursNonZero;
                } else if (UtilityRoutines::SameString(curAggString, "HoursPositive")) {
                    curAggType = iAggType::HoursPositive;
                } else if (UtilityRoutines::SameString(curAggString, "HoursNonpositive")) {
                    curAggType = iAggType::HoursNonPositive;
                } else if (UtilityRoutines::SameString(curAggString, "HoursNegative")) {
                    curAggType = iAggType::HoursNegative;
                } else if (UtilityRoutines::SameString(curAggString, "HoursNonnegative")) {
                    curAggType = iAggType::HoursNonNegative;
                } else if (UtilityRoutines::SameString(curAggString, "SumOrAverageDuringHoursShown")) {
                    curAggType = iAggType::SumOrAverageHoursShown;
                } else if (UtilityRoutines::SameString(curAggString, "MaximumDuringHoursShown")) {
                    curAggType = iAggType::MaximumDuringHoursShown;
                } else if (UtilityRoutines::SameString(curAggString, "MinimumDuringHoursShown")) {
                    curAggType = iAggType::MinimumDuringHoursShown;
                } else {
                    curAggType = iAggType::SumOrAvg;
                    ShowWarningError(state, CurrentModuleObject + '=' + ort->MonthlyInput(TabNum).name + ", Variable name=" + AlphArray(jField));
                    ShowContinueError(state, "Invalid aggregation type=\"" + curAggString + "\"  Defaulting to SumOrAverage.");
                }
                AddMonthlyFieldSetInput(state, curTable, AlphArray(jField), "", curAggType);
            }
        }
    }

    int AddMonthlyReport(EnergyPlusData &state, std::string const &inReportName, int const inNumDigitsShown)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   August 2008
        //       MODIFIED
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Creates a monthly report

        // METHODOLOGY EMPLOYED:
        // na

        // REFERENCES:
        // na

        // USE STATEMENTS:

        // Return value

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int const SizeAdder(25);
        auto &ort(state.dataOutRptTab);

        if (!allocated(ort->MonthlyInput)) {
            ort->MonthlyInput.allocate(SizeAdder);
            ort->sizeMonthlyInput = SizeAdder;
            ort->MonthlyInputCount = 1;
        } else {
            ++ort->MonthlyInputCount;
            // if larger than current size grow the array
            if (ort->MonthlyInputCount > ort->sizeMonthlyInput) {
                ort->MonthlyInput.redimension(ort->sizeMonthlyInput += SizeAdder);
            }
        }
        // initialize new record
        ort->MonthlyInput(ort->MonthlyInputCount).name = inReportName;
        ort->MonthlyInput(ort->MonthlyInputCount).showDigits = inNumDigitsShown;
        return ort->MonthlyInputCount;
    }

    void AddMonthlyFieldSetInput(EnergyPlusData &state, int const inMonthReport, std::string const &inVariMeter, std::string const &inColHead, iAggType const inAggregate)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   August 2008
        //       MODIFIED
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Assigns the column information for predefined
        //   monthly reports

        // METHODOLOGY EMPLOYED:
        //   Simple assignments to public variables.

        // REFERENCES:
        // na

        // USE STATEMENTS:

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        int const sizeIncrement(50);
        auto &ort(state.dataOutRptTab);

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        // na

        if (!allocated(ort->MonthlyFieldSetInput)) {
            ort->MonthlyFieldSetInput.allocate(sizeIncrement);
            ort->sizeMonthlyFieldSetInput = sizeIncrement;
            ort->MonthlyFieldSetInputCount = 1;
        } else {
            ++ort->MonthlyFieldSetInputCount;
            // if larger than current size grow the array
            if (ort->MonthlyFieldSetInputCount > ort->sizeMonthlyFieldSetInput) {
                ort->MonthlyFieldSetInput.redimension(ort->sizeMonthlyFieldSetInput *=
                                                 2); // Tuned Changed += sizeIncrement to *= 2 for reduced heap allocations (at some space cost)
            }
        }
        // initialize new record)
        ort->MonthlyFieldSetInput(ort->MonthlyFieldSetInputCount).variMeter = inVariMeter;
        ort->MonthlyFieldSetInput(ort->MonthlyFieldSetInputCount).colHead = inColHead;
        ort->MonthlyFieldSetInput(ort->MonthlyFieldSetInputCount).aggregate = inAggregate;
        // update the references from the MonthlyInput array
        if ((inMonthReport > 0) && (inMonthReport <= ort->MonthlyInputCount)) {
            if (ort->MonthlyInput(inMonthReport).firstFieldSet == 0) {
                ort->MonthlyInput(inMonthReport).firstFieldSet = ort->MonthlyFieldSetInputCount;
                ort->MonthlyInput(inMonthReport).numFieldSet = 1;
            } else {
                ++ort->MonthlyInput(inMonthReport).numFieldSet;
            }
        }
    }

    void InitializeTabularMonthly(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   July 2008
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   This routine initializes the data structures based
        //   on input from either the IDF file or from the predefined
        //   monthly reports.  The data structures follow the IDD
        //   closely.  The routine initializes many of the arrays
        //   for monthly tables.

        // METHODOLOGY EMPLOYED:
        //   Process the data structures that define monthly tabular
        //   reports

        // NOTE:
        //   The bulk of this routine used to be part of the the
        //   GetInputTabularMonthly routine but when predefined
        //   monthly reports were added this routine was seperated
        //   from input.

        // REFERENCES:
        // na

        // USE STATEMENTS:
        // na

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int TabNum;               // index when cycling through each table
        int NumColumns;           // number of columns specified in the input for an object
        int FirstColumn;          // the first column of the monthly input
        std::string curVariMeter; // current variable or meter
        int colNum;               // loop index for columns
        int KeyCount;
        int TypeVar;
        OutputProcessor::StoreType AvgSumVar;
        OutputProcessor::TimeStepType StepTypeVar;
        OutputProcessor::Unit UnitsVar(OutputProcessor::Unit::None); // Units enum
        Array1D_string UniqueKeyNames;
        int UniqueKeyCount;
        int iKey;
        int jUnique;
        int found;
        int kUniqueKey;
        int lTable;
        int mColumn;
        int ColumnsRecount;
        int TablesRecount;
        static Real64 BigNum(0.0);
        bool environmentKeyFound;
        static bool VarWarning(true);
        static int ErrCount1(0);
        auto &ort(state.dataOutRptTab);

        // if not a running a weather simulation do not create reports
        if (!state.dataGlobal->DoWeathSim) return;
        ort->maxUniqueKeyCount = 1500;
        UniqueKeyNames.allocate(ort->maxUniqueKeyCount);
        // First pass through the input objects is to put the name of the report
        // into the array and count the number of unique keys found to allocate
        // the monthlyTables and monthlyColumns
        // This approach seems inefficient but I know of no other way to size
        // the arrays prior to filling them and to size the arrays basically
        // the same steps must be gone through as with filling the arrays.

        //#ifdef ITM_KEYCACHE
        // Noel comment:  How about allocating these variables once for the whole routine?
        //    Again, if a max value for key count can be agreed upon, we could use it here --
        //    otherwise, will have to have re-allocate logic.
        // maxKeyCount=1500 ! ?
        // ALLOCATE(NamesOfKeys(maxKeyCount))
        // ALLOCATE(IndexesForKeyVar(maxKeyCount))
        //#endif

        ort->MonthlyColumnsCount = 0;
        ort->MonthlyTablesCount = 0;
        for (TabNum = 1; TabNum <= ort->MonthlyInputCount; ++TabNum) {
            // the number of columns based on number of alpha fields
            NumColumns = ort->MonthlyInput(TabNum).numFieldSet;
            FirstColumn = ort->MonthlyInput(TabNum).firstFieldSet;
            environmentKeyFound = false;
            UniqueKeyCount = 0;
            for (colNum = 1; colNum <= NumColumns; ++colNum) {

                //#ifdef ITM_KEYCACHE
                // Noel comment:  First time in this TabNum/ColNum loop, let's save the results
                //  of GetVariableKeyCountandType & GetVariableKeys.
                curVariMeter = UtilityRoutines::MakeUPPERCase(ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).variMeter);
                // call the key count function but only need count during this pass
                GetVariableKeyCountandType(state, curVariMeter, KeyCount, TypeVar, AvgSumVar, StepTypeVar, UnitsVar);
                if (TypeVar == OutputProcessor::VarType_NotFound) {
                    ShowWarningError(state, "In Output:Table:Monthly '" +  ort->MonthlyInput(TabNum).name + "' invalid Variable or Meter Name '" + curVariMeter + "'");
                }
                //    IF (KeyCount > maxKeyCount) THEN
                //      DEALLOCATE(NamesOfKeys)
                //      DEALLOCATE(IndexesForKeyVar)
                //      maxKeyCount=KeyCount
                //      ALLOCATE(NamesOfKeys(maxKeyCount))
                //      ALLOCATE(IndexesForKeyVar(maxKeyCount))
                //    ENDIF
                ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).NamesOfKeys.allocate(KeyCount);
                ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).IndexesForKeyVar.allocate(KeyCount);

                // fill keys?
                GetVariableKeys(state, curVariMeter,
                                TypeVar,
                                ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).NamesOfKeys,
                                ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).IndexesForKeyVar);

                // save these values to use later -- noel
                ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).variMeterUpper = curVariMeter;
                ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).typeOfVar = TypeVar;
                ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).keyCount = KeyCount;
                ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).varAvgSum = AvgSumVar;
                ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).varStepType = StepTypeVar;
                ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).varUnits = UnitsVar;
                //    DO iKey = 1, KeyCount
                //      MonthlyFieldSetInput(FirstColumn + ColNum - 1)%NamesOfKeys(iKey) = NamesOfKeys(iKey)  !noel
                //      MonthlyFieldSetInput(FirstColumn + ColNum - 1)%IndexesForKeyVar(iKey) = IndexesForKeyVar(iKey)  !noel
                //    ENDDO
                //#else
                //    curVariMeter = UtilityRoutines::MakeUPPERCase(MonthlyFieldSetInput(FirstColumn + ColNum - 1)%variMeter)
                //    ! call the key count function but only need count during this pass
                //    CALL GetVariableKeyCountandType(state, curVariMeter,KeyCount,TypeVar,AvgSumVar,StepTypeVar,UnitsVar)
                //    ALLOCATE(NamesOfKeys(KeyCount))
                //    ALLOCATE(IndexesForKeyVar(KeyCount))
                //    CALL GetVariableKeys(state, curVariMeter,TypeVar,NamesOfKeys,IndexesForKeyVar)
                //#endif

                for (iKey = 1; iKey <= KeyCount; ++iKey) {
                    found = 0;
                    // set a flag if environment variables are found
                    if (UtilityRoutines::SameString(ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).NamesOfKeys(iKey), "ENVIRONMENT")) {
                        environmentKeyFound = true;
                        found = -1; // so not counted in list of unique keys
                    }
                    for (jUnique = 1; jUnique <= UniqueKeyCount; ++jUnique) {
                        if (UtilityRoutines::SameString(UniqueKeyNames(jUnique), ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).NamesOfKeys(iKey))) {
                            found = jUnique;
                            break;
                        }
                    }
                    if (found == 0) {
                        ++UniqueKeyCount;
                        if (UniqueKeyCount > ort->maxUniqueKeyCount) {
                            UniqueKeyNames.redimension(ort->maxUniqueKeyCount += 500);
                        }
                        UniqueKeyNames(UniqueKeyCount) = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).NamesOfKeys(iKey);
                    }
                }
                //#ifdef ITM_KEYCACHE
                //    ! Don't deallocate here, only allocating/deallocating once for the whole routine
                //#else
                //    DEALLOCATE(NamesOfKeys)
                //    DEALLOCATE(IndexesForKeyVar)
                //#endif
            } // colNum
            // fix for CR8285 - when monthly report is only environmental variables
            if (environmentKeyFound && UniqueKeyCount == 0) {
                UniqueKeyCount = 1;
            }
            // increment the number of tables based on the number of unique keys
            ort->MonthlyTablesCount += UniqueKeyCount;
            ort->MonthlyColumnsCount += UniqueKeyCount * NumColumns;
        } // TabNum the end of the loop through the inputs objects
        // Now that we have the maximum size of the number of tables (each table is
        // repeated for the number of keys found) and the number of total columns
        // of all of the tables, allocate the arrays to store this information.
        ort->MonthlyTables.allocate(ort->MonthlyTablesCount);
        ort->MonthlyColumns.allocate(ort->MonthlyColumnsCount);
        // Initialize tables and results
        for (auto &e : ort->MonthlyTables) {
            e.keyValue.clear();
            e.firstColumn = 0;
            e.numColumns = 0;
        }

        for (auto &e : ort->MonthlyColumns) {
            e.varName.clear();
            e.varNum = 0;
            e.typeOfVar = 0;
            e.avgSum = OutputProcessor::StoreType::Averaged;
            e.stepType = OutputProcessor::TimeStepType::TimeStepZone;
            e.units = OutputProcessor::Unit::None;
            e.aggType = iAggType::Unassigned;
        }
        for (colNum = 1; colNum <= ort->MonthlyColumnsCount; ++colNum) {
            ort->MonthlyColumns(colNum).reslt = 0.0;
            ort->MonthlyColumns(colNum).timeStamp = 0;
            ort->MonthlyColumns(colNum).duration = 0.0;
        }

        ColumnsRecount = 0;
        TablesRecount = 0;
        for (TabNum = 1; TabNum <= ort->MonthlyInputCount; ++TabNum) {
            // the number of columns based on number of alpha fields
            NumColumns = ort->MonthlyInput(TabNum).numFieldSet;
            FirstColumn = ort->MonthlyInput(TabNum).firstFieldSet;
            UniqueKeyCount = 0;
            environmentKeyFound = false;
            for (colNum = 1; colNum <= NumColumns; ++colNum) {
                //#ifdef ITM_KEYCACHE
                // Noel comment:  Here is where we could use the saved values
                curVariMeter = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).variMeterUpper;
                KeyCount = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).keyCount;
                TypeVar = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).typeOfVar;
                AvgSumVar = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).varAvgSum;
                StepTypeVar = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).varStepType;
                UnitsVar = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).varUnits;
                //    DO iKey = 1, KeyCount  !noel
                //       NamesOfKeys(iKey) = MonthlyFieldSetInput(FirstColumn + ColNum - 1)%NamesOfKeys(iKey)  !noel
                //       IndexesForKeyVar(iKey) = MonthlyFieldSetInput(FirstColumn + ColNum - 1)%IndexesForKeyVar(iKey) !noel
                //    ENDDO
                //#else
                //    curVariMeter = UtilityRoutines::MakeUPPERCase(MonthlyFieldSetInput(FirstColumn + ColNum - 1)%variMeter)
                //    ! call the key count function but only need count during this pass
                //    CALL GetVariableKeyCountandType(state, curVariMeter,KeyCount,TypeVar,AvgSumVar,StepTypeVar,UnitsVar)
                //    ALLOCATE(NamesOfKeys(KeyCount))
                //    ALLOCATE(IndexesForKeyVar(KeyCount))
                //    CALL GetVariableKeys(state, curVariMeter,TypeVar,NamesOfKeys,IndexesForKeyVar)
                //#endif

                if (KeyCount == 0) {
                    ++ErrCount1;
                    if (ErrCount1 == 1 && !state.dataGlobal->DisplayExtraWarnings && state.dataGlobal->KindOfSim == DataGlobalConstants::KindOfSim::RunPeriodWeather) {
                        ShowWarningError(state, "Processing Monthly Tabular Reports: Variable names not valid for this simulation");
                        ShowContinueError(state, "...use Output:Diagnostics,DisplayExtraWarnings; to show more details on individual variables.");
                    }
                    // fixing CR5878 removed the showing of the warning once about a specific variable.
                    if (state.dataGlobal->DisplayExtraWarnings && state.dataGlobal->KindOfSim == DataGlobalConstants::KindOfSim::RunPeriodWeather) {
                        ShowWarningError(state, "Processing Monthly Tabular Reports: " + ort->MonthlyInput(TabNum).name);
                        ShowContinueError(state, "..Variable name=" + curVariMeter + " not valid for this simulation.");
                        if (VarWarning) {
                            ShowContinueError(state, "..Variables not valid for this simulation will have \"[Invalid/Undefined]\" in the Units Column of "
                                              "the Table Report.");
                            VarWarning = false;
                        }
                    }
                }
                for (iKey = 1; iKey <= KeyCount; ++iKey) {
                    found = 0;
                    // set a flag if environment variables are found
                    if (UtilityRoutines::SameString(ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).NamesOfKeys(iKey), "ENVIRONMENT")) {
                        environmentKeyFound = true;
                        found = -1; // so not counted in list of unique keys
                    }
                    for (jUnique = 1; jUnique <= UniqueKeyCount; ++jUnique) {
                        if (UtilityRoutines::SameString(UniqueKeyNames(jUnique), ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).NamesOfKeys(iKey))) {
                            found = jUnique;
                            break;
                        }
                    }
                    if (found == 0) {
                        ++UniqueKeyCount;
                        UniqueKeyNames(UniqueKeyCount) = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).NamesOfKeys(iKey);
                    }
                }
                //#ifdef ITM_KEYCACHE
                //    ! Don't deallocate here, only allocating/deallocating once for the whole routine
                //#else
                //    DEALLOCATE(NamesOfKeys)
                //    DEALLOCATE(IndexesForKeyVar)
                //#endif
            }
            // fix for CR8285 - when monthly report is only environmental variables
            if (environmentKeyFound && UniqueKeyCount == 0) {
                UniqueKeyCount = 1;
            }
            // increment the number of tables based on the number of unique keys
            ort->MonthlyInput(TabNum).firstTable = TablesRecount + 1;
            ort->MonthlyInput(TabNum).numTables = UniqueKeyCount;
            TablesRecount += UniqueKeyCount;
            // loop through the different unique keys since each user defined table
            // has that many instances - one for each unique key.
            // It is unusual that this loop is about 'keys' and an inner loop is also
            // about 'keys' but for this loop the keys are really instances of tables.
            for (kUniqueKey = 1; kUniqueKey <= UniqueKeyCount; ++kUniqueKey) {
                lTable = kUniqueKey + ort->MonthlyInput(TabNum).firstTable - 1;
                // use the term 'environment' for identifying the report if
                if (environmentKeyFound && UniqueKeyCount == 1) {
                    ort->MonthlyTables(lTable).keyValue = "Environment";
                } else { // this is the most common case is to use the unique key for the report
                    ort->MonthlyTables(lTable).keyValue = UniqueKeyNames(kUniqueKey);
                }
                ort->MonthlyTables(lTable).firstColumn = ColumnsRecount + 1;
                ort->MonthlyTables(lTable).numColumns = NumColumns;
                ColumnsRecount += NumColumns;
                FirstColumn = ort->MonthlyInput(TabNum).firstFieldSet;
                for (colNum = 1; colNum <= NumColumns; ++colNum) {
                    environmentKeyFound = false;
                    mColumn = colNum + ort->MonthlyTables(lTable).firstColumn - 1;
                    // when going through the columns this time, not all columns may have
                    // a EP variable that corresponds to it.  In no variable is found
                    // then set it to 0 to be skipped during data gathering

                    //#ifdef ITM_KEYCACHE
                    // Noel comment:  Here is where we could use the saved values
                    curVariMeter = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).variMeterUpper;
                    KeyCount = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).keyCount;
                    TypeVar = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).typeOfVar;
                    AvgSumVar = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).varAvgSum;
                    StepTypeVar = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).varStepType;
                    UnitsVar = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).varUnits;
                    //    DO iKey = 1, KeyCount  !noel
                    //       NamesOfKeys(iKey) = MonthlyFieldSetInput(FirstColumn + ColNum - 1)%NamesOfKeys(iKey)  !noel
                    //       IndexesForKeyVar(iKey) = MonthlyFieldSetInput(FirstColumn + ColNum - 1)%IndexesForKeyVar(iKey) !noel
                    //    ENDDO
                    //#else
                    //    curVariMeter = UtilityRoutines::MakeUPPERCase(MonthlyFieldSetInput(FirstColumn + ColNum - 1)%variMeter)
                    //    ! call the key count function but only need count during this pass
                    //    CALL GetVariableKeyCountandType(state, curVariMeter,KeyCount,TypeVar,AvgSumVar,StepTypeVar,UnitsVar)
                    //    ALLOCATE(NamesOfKeys(KeyCount))
                    //    ALLOCATE(IndexesForKeyVar(KeyCount))
                    //    CALL GetVariableKeys(state, curVariMeter,TypeVar,NamesOfKeys,IndexesForKeyVar)
                    //#endif

                    if (KeyCount == 1) { // first test if KeyCount is one to avoid referencing a zero element array
                        if (UtilityRoutines::SameString(ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).NamesOfKeys(1), "ENVIRONMENT")) {
                            environmentKeyFound = true;
                        }
                    }
                    // if this is an environment variable - don't bother searching
                    if (environmentKeyFound) {
                        found = 1; // only one instance of environment variables so use it.
                    } else {
                        // search through the keys for the currently active key "UniqueKeyNames(kUniqueKey)"
                        found = 0;
                        for (iKey = 1; iKey <= KeyCount; ++iKey) {
                            if (UtilityRoutines::SameString(ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).NamesOfKeys(iKey),
                                                            UniqueKeyNames(kUniqueKey))) {
                                found = iKey;
                                break;
                            }
                        }
                    }
                    if ((found > 0) && (KeyCount >= 1)) {
                        ort->MonthlyColumns(mColumn).varName = curVariMeter;
                        ort->MonthlyColumns(mColumn).varNum = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).IndexesForKeyVar(found);
                        ort->MonthlyColumns(mColumn).typeOfVar = TypeVar;
                        ort->MonthlyColumns(mColumn).avgSum = AvgSumVar;
                        ort->MonthlyColumns(mColumn).stepType = StepTypeVar;
                        ort->MonthlyColumns(mColumn).units = UnitsVar;
                        ort->MonthlyColumns(mColumn).aggType = ort->MonthlyFieldSetInput(FirstColumn + colNum - 1).aggregate;
                        // set accumulator values to default as appropriate for aggregation type
                        {
                            auto const SELECT_CASE_var(ort->MonthlyColumns(mColumn).aggType);
                            if (SELECT_CASE_var == iAggType::SumOrAvg) {
                                ort->MonthlyColumns(mColumn).reslt = 0.0;
                                ort->MonthlyColumns(mColumn).duration = 0.0;
                            } else if (SELECT_CASE_var == iAggType::Maximum) {
                                ort->MonthlyColumns(mColumn).reslt = -HUGE_(BigNum);
                                ort->MonthlyColumns(mColumn).timeStamp = 0;
                            } else if (SELECT_CASE_var == iAggType::Minimum) {
                                ort->MonthlyColumns(mColumn).reslt = HUGE_(BigNum);
                                ort->MonthlyColumns(mColumn).timeStamp = 0;
                            } else if (SELECT_CASE_var == iAggType::ValueWhenMaxMin) {
                                ort->MonthlyColumns(mColumn).reslt = 0.0;
                            } else if (SELECT_CASE_var == iAggType::HoursZero) {
                                ort->MonthlyColumns(mColumn).reslt = 0.0;
                            } else if (SELECT_CASE_var == iAggType::HoursNonZero) {
                                ort->MonthlyColumns(mColumn).reslt = 0.0;
                            } else if (SELECT_CASE_var == iAggType::HoursPositive) {
                                ort->MonthlyColumns(mColumn).reslt = 0.0;
                            } else if (SELECT_CASE_var == iAggType::HoursNonPositive) {
                                ort->MonthlyColumns(mColumn).reslt = 0.0;
                            } else if (SELECT_CASE_var == iAggType::HoursNegative) {
                                ort->MonthlyColumns(mColumn).reslt = 0.0;
                            } else if (SELECT_CASE_var == iAggType::HoursNonNegative) {
                                ort->MonthlyColumns(mColumn).reslt = 0.0;
                            } else if (SELECT_CASE_var == iAggType::SumOrAverageHoursShown) {
                                ort->MonthlyColumns(mColumn).reslt = 0.0;
                                ort->MonthlyColumns(mColumn).duration = 0.0;
                            } else if (SELECT_CASE_var == iAggType::MaximumDuringHoursShown) {
                                ort->MonthlyColumns(mColumn).reslt = -HUGE_(BigNum);
                                ort->MonthlyColumns(mColumn).timeStamp = 0;
                            } else if (SELECT_CASE_var == iAggType::MinimumDuringHoursShown) {
                                ort->MonthlyColumns(mColumn).reslt = HUGE_(BigNum);
                                ort->MonthlyColumns(mColumn).timeStamp = 0;
                            }
                        }
                    } else { // if no key corresponds to this instance of the report
                        // fixing CR5878 removed the showing of the warning once about a specific variable.
                        if (state.dataGlobal->DisplayExtraWarnings && state.dataGlobal->KindOfSim == DataGlobalConstants::KindOfSim::RunPeriodWeather) {
                            ShowWarningError(state, "Processing Monthly Tabular Reports: " + ort->MonthlyInput(TabNum).name);
                            ShowContinueError(state, "..Variable name=" + curVariMeter + " not valid for this simulation.");
                            ShowContinueError(state, "..i.e., Variable name=" + UniqueKeyNames(kUniqueKey) + ':' + curVariMeter +
                                              " not valid for this simulation.");
                            if (VarWarning) {
                                ShowContinueError(state, "..Variables not valid for this simulation will have \"[Invalid/Undefined]\" in the Units Column "
                                                  "of the Table Report.");
                                VarWarning = false;
                            }
                        }
                        ort->MonthlyColumns(mColumn).varName = curVariMeter;
                        ort->MonthlyColumns(mColumn).varNum = 0;
                        ort->MonthlyColumns(mColumn).typeOfVar = 0;
                        ort->MonthlyColumns(mColumn).avgSum = OutputProcessor::StoreType::Averaged;
                        ort->MonthlyColumns(mColumn).stepType = OutputProcessor::TimeStepType::TimeStepZone;
                        ort->MonthlyColumns(mColumn).units = OutputProcessor::Unit::None;
                        ort->MonthlyColumns(mColumn).aggType = iAggType::SumOrAvg;
                    }
                    //#ifdef ITM_KEYCACHE
                    //#else
                    //    DEALLOCATE(NamesOfKeys)
                    //    DEALLOCATE(IndexesForKeyVar)
                    //#endif
                } // ColNum
            }     // kUniqueKey
        }         // TabNum the end of the loop through the inputs objects

        //#ifdef ITM_KEYCACHE
        // DEALLOCATE(NamesOfKeys)
        // DEALLOCATE(IndexesForKeyVar)
        //#endif
    }

    bool isInvalidAggregationOrder(EnergyPlusData &state)
    {
        auto &ort(state.dataOutRptTab);
        bool foundError = false;
        if (!state.dataGlobal->DoWeathSim) { // if no weather simulation than no reading of MonthlyInput array
            return foundError;
        }
        for (int iInput = 1; iInput <= ort->MonthlyInputCount; ++iInput) {
            bool foundMinOrMax = false;
            bool foundHourAgg = false;
            bool missingMaxOrMinError = false;
            bool missingHourAggError = false;
            for (int jTable = 1; jTable <= ort->MonthlyInput(iInput).numTables; ++jTable) {
                int curTable = jTable + ort->MonthlyInput(iInput).firstTable - 1;
                // test if the aggregation types are in the correct order
                for (int kColumn = 1; kColumn <= ort->MonthlyTables(curTable).numColumns; ++kColumn) {
                    int curCol = kColumn + ort->MonthlyTables(curTable).firstColumn - 1;
                    if (ort->MonthlyColumns(curCol).varNum == 0) break; // if no variable was ever found than stop checking
                    iAggType curAggType = ort->MonthlyColumns(curCol).aggType;
                    if ((curAggType == iAggType::Maximum) || (curAggType == iAggType::Minimum)) {
                        foundMinOrMax = true;
                    } else if ((curAggType == iAggType::HoursNonZero) || (curAggType == iAggType::HoursZero) || (curAggType == iAggType::HoursPositive) ||
                               (curAggType == iAggType::HoursNonPositive) || (curAggType == iAggType::HoursNegative) ||
                               (curAggType == iAggType::HoursNonNegative)) {
                        foundHourAgg = true;
                    } else if (curAggType == iAggType::ValueWhenMaxMin) {
                        if (!foundMinOrMax) {
                            missingMaxOrMinError = true;
                        }
                    } else if ((curAggType == iAggType::SumOrAverageHoursShown) || (curAggType == iAggType::MaximumDuringHoursShown) ||
                               (curAggType == iAggType::MinimumDuringHoursShown)) {
                        if (!foundHourAgg) {
                            missingHourAggError = true;
                        }
                    }
                }
            }
            if (missingMaxOrMinError) {
                ShowSevereError(state, "The Output:Table:Monthly report named=\"" + ort->MonthlyInput(iInput).name +
                                "\" has a valueWhenMaxMin aggregation type for a column without a previous column that uses either the minimum or "
                                "maximum aggregation types. The report will not be generated.");
                foundError = true;
            }
            if (missingHourAggError) {
                ShowSevereError(state, "The Output:Table:Monthly report named=\"" + ort->MonthlyInput(iInput).name +
                                "\" has a --DuringHoursShown aggregation type for a column without a previous field that uses one of the Hour-- "
                                "aggregation types. The report will not be generated.");
                foundError = true;
            }
        }
        return foundError;
    }

    void GetInputTabularTimeBins(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   July 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   This routine initializes the data structures based
        //   on input from in the IDF file.  The data structures
        //   follow the IDD closely.

        // METHODOLOGY EMPLOYED:
        //   Uses get input structure similar to other objects

        // REFERENCES:
        // na

        // Using/Aliasing
        using namespace DataIPShortCuts;
        using ScheduleManager::GetScheduleIndex;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        static std::string const CurrentModuleObject("Output:Table:TimeBins");

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int iInObj;               // index when cycling through each idf input object
        int NumParams;            // Number of elements combined
        int NumAlphas;            // Number of elements in the alpha array
        int NumNums;              // Number of elements in the numeric array
        Array1D_string AlphArray; // character string data
        Array1D<Real64> NumArray; // numeric data
        int IOStat;               // IO Status when calling get input subroutine
        int iTable;
        int firstReport;
        int repIndex;
        int found;
        Real64 const bigVal(0.0); // used with HUGE: Value doesn't matter, only type: Initialize so compiler doesn't warn about use uninitialized

        Array1D_string objNames;
        Array1D_int objVarIDs;
        auto &ort(state.dataOutRptTab);

        if (!(state.files.outputControl.tabular || state.files.outputControl.sqlite)) {
            ort->WriteTabularFiles = false;
            return;
        }

        inputProcessor->getObjectDefMaxArgs(state, CurrentModuleObject, NumParams, NumAlphas, NumNums);
        AlphArray.allocate(NumAlphas);
        NumArray.dimension(NumNums, 0.0);

        ort->timeInYear = 0.0; // initialize the time in year counter
        // determine size of array that holds the IDF description
        ort->OutputTableBinnedCount = inputProcessor->getNumObjectsFound(state, CurrentModuleObject);
        ort->OutputTableBinned.allocate(ort->OutputTableBinnedCount);
        if (ort->OutputTableBinnedCount > 0) {
            ort->WriteTabularFiles = true;
            // if not a run period using weather do not create reports
            if (!state.dataGlobal->DoWeathSim) {
                ShowWarningError(state, CurrentModuleObject + " requested with SimulationControl Run Simulation for Weather File Run Periods set to No so " +
                                 CurrentModuleObject + " will not be generated");
                return;
            }
        }
        // looking for maximum number of intervals for sizing
        ort->BinResultsIntervalCount = 0;
        ort->BinResultsTableCount = 0;
        for (iInObj = 1; iInObj <= ort->OutputTableBinnedCount; ++iInObj) {
            inputProcessor->getObjectItem(state,
                                          CurrentModuleObject,
                                          iInObj,
                                          AlphArray,
                                          NumAlphas,
                                          NumArray,
                                          NumNums,
                                          IOStat,
                                          lNumericFieldBlanks,
                                          lAlphaFieldBlanks,
                                          cAlphaFieldNames,
                                          cNumericFieldNames);
            ort->OutputTableBinned(iInObj).keyValue = AlphArray(1);
            ort->OutputTableBinned(iInObj).varOrMeter = AlphArray(2);
            // if a schedule has been specified assign
            if (len(AlphArray(3)) > 0) {
                ort->OutputTableBinned(iInObj).ScheduleName = AlphArray(3);
                ort->OutputTableBinned(iInObj).scheduleIndex = GetScheduleIndex(state, AlphArray(3));
                if (ort->OutputTableBinned(iInObj).scheduleIndex == 0) {
                    ShowWarningError(state, CurrentModuleObject + ": invalid " + cAlphaFieldNames(3) + "=\"" + AlphArray(3) + "\" - not found.");
                }
            } else {
                ort->OutputTableBinned(iInObj).scheduleIndex = 0; // flag value for no schedule used
            }
            // validate the kind of variable - not used internally except for validation
            if (len(AlphArray(4)) > 0) {
                if (!(UtilityRoutines::SameString(AlphArray(4), "ENERGY") || UtilityRoutines::SameString(AlphArray(4), "DEMAND") ||
                      UtilityRoutines::SameString(AlphArray(4), "TEMPERATURE") || UtilityRoutines::SameString(AlphArray(4), "FLOWRATE"))) {
                    ShowWarningError(state, "In " + CurrentModuleObject + " named " + AlphArray(1) +
                                     " the Variable Type was not energy, demand, temperature, or flowrate.");
                }
            }
            ort->OutputTableBinned(iInObj).intervalStart = NumArray(1);
            ort->OutputTableBinned(iInObj).intervalSize = NumArray(2);
            ort->OutputTableBinned(iInObj).intervalCount = int(NumArray(3));
            // valid range checking on inputs
            if (ort->OutputTableBinned(iInObj).intervalCount < 1) {
                ort->OutputTableBinned(iInObj).intervalCount = 1;
            }
            if (ort->OutputTableBinned(iInObj).intervalCount > 20) {
                ort->OutputTableBinned(iInObj).intervalCount = 20;
            }
            if (ort->OutputTableBinned(iInObj).intervalSize < 0) {
                ort->OutputTableBinned(iInObj).intervalSize = 1000.0;
            }
            ort->OutputTableBinned(iInObj).resIndex = ort->BinResultsTableCount + 1; // the next results report
            // find maximum number of intervals
            if (ort->OutputTableBinned(iInObj).intervalCount > ort->BinResultsIntervalCount) {
                ort->BinResultsIntervalCount = ort->OutputTableBinned(iInObj).intervalCount;
            }
            GetVariableKeyCountandType(state, ort->OutputTableBinned(iInObj).varOrMeter,
                                       ort->OutputTableBinned(iInObj).numTables,
                                       ort->OutputTableBinned(iInObj).typeOfVar,
                                       ort->OutputTableBinned(iInObj).avgSum,
                                       ort->OutputTableBinned(iInObj).stepType,
                                       ort->OutputTableBinned(iInObj).units);
            if (ort->OutputTableBinned(iInObj).typeOfVar == 0) {
                ShowWarningError(state, CurrentModuleObject + ": User specified meter or variable not found: " + ort->OutputTableBinned(iInObj).varOrMeter);
            }
            // If only a single table key is requested than only one should be counted
            // later will reset the numTables array pointer but for now use it to know
            // how many items to scan through
            if (ort->OutputTableBinned(iInObj).keyValue == "*") {
                ort->BinResultsTableCount += ort->OutputTableBinned(iInObj).numTables;
            } else {
                ++ort->BinResultsTableCount; // if a particular key is requested then only one more report
            }
        }
        // size the arrays that holds the bin results
        ort->BinResults.allocate(ort->BinResultsIntervalCount, ort->BinResultsTableCount);
        ort->BinResultsBelow.allocate(ort->BinResultsTableCount);
        ort->BinResultsAbove.allocate(ort->BinResultsTableCount);
        ort->BinStatistics.allocate(ort->BinResultsTableCount);
        ort->BinObjVarID.allocate(ort->BinResultsTableCount);
        // now that the arrays are sized go back and fill in
        // what ID numbers are used for each table
        for (iInObj = 1; iInObj <= ort->OutputTableBinnedCount; ++iInObj) {
            firstReport = ort->OutputTableBinned(iInObj).resIndex;
            // allocate the arrays to the number of objects
            objNames.allocate(ort->OutputTableBinned(iInObj).numTables);
            objVarIDs.allocate(ort->OutputTableBinned(iInObj).numTables);
            GetVariableKeys(state, ort->OutputTableBinned(iInObj).varOrMeter, ort->OutputTableBinned(iInObj).typeOfVar, objNames, objVarIDs);
            if (ort->OutputTableBinned(iInObj).keyValue == "*") {
                for (iTable = 1; iTable <= ort->OutputTableBinned(iInObj).numTables; ++iTable) {
                    repIndex = firstReport + (iTable - 1);
                    ort->BinObjVarID(repIndex).namesOfObj = objNames(iTable);
                    ort->BinObjVarID(repIndex).varMeterNum = objVarIDs(iTable);
                    // check if valid meter or number
                    if (objVarIDs(iTable) == 0) {
                        ShowWarningError(state, CurrentModuleObject + ": Specified variable or meter not found: " + objNames(iTable));
                    }
                }
            } else {
                // scan through the keys and look for the user specified key
                found = 0;
                for (iTable = 1; iTable <= ort->OutputTableBinned(iInObj).numTables; ++iTable) {
                    if (UtilityRoutines::SameString(objNames(iTable), ort->OutputTableBinned(iInObj).keyValue)) {
                        found = iTable;
                        break;
                    }
                }
                // the first and only report is assigned to the found object name
                if (!warningAboutKeyNotFound(state, found, iInObj, CurrentModuleObject)) {
                    ort->BinObjVarID(firstReport).namesOfObj = objNames(found);
                    ort->BinObjVarID(firstReport).varMeterNum = objVarIDs(found);
                }
                // reset the number of tables to one
                ort->OutputTableBinned(iInObj).numTables = 1;
            }
        }
        // clear the binning arrays to zeros
        for (auto &e : ort->BinResults) {
            e.mnth = 0.0;
            e.hrly = 0.0;
        }
        for (auto &e : ort->BinResultsBelow) {
            e.mnth = 0.0;
            e.hrly = 0.0;
        }
        for (auto &e : ort->BinResultsAbove) {
            e.mnth = 0.0;
            e.hrly = 0.0;
        }

        // initialize statistics counters
        for (auto &e : ort->BinStatistics) {
            e.minimum = HUGE_(bigVal);
            e.maximum = -HUGE_(bigVal);
            e.n = 0;
            e.sum = 0.0;
            e.sum2 = 0.0;
        }
    }

    bool warningAboutKeyNotFound(EnergyPlusData &state, int foundIndex, int inObjIndex, std::string const &moduleName)
    {
        if (foundIndex == 0) {
            ShowWarningError(state, moduleName + ": Specified key not found: " + state.dataOutRptTab->OutputTableBinned(inObjIndex).keyValue +
                             " for variable: " + state.dataOutRptTab->OutputTableBinned(inObjIndex).varOrMeter);
            return true;
        } else {
            return false;
        }
    }

    void GetInputTabularStyle(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   July 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   This routine set a flag for the output format for
        //   all tabular reports. This is a "unique" object.

        // METHODOLOGY EMPLOYED:
        //   Uses get input structure similar to other objects

        // REFERENCES:
        // na

        // Using/Aliasing
        using namespace DataIPShortCuts;
        using DataStringGlobals::CharComma;
        using DataStringGlobals::CharSpace;
        using DataStringGlobals::CharTab;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        static std::string const CurrentModuleObject("OutputControl:Table:Style");

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int NumTabularStyle;
        int NumParams;            // Number of elements combined
        int NumAlphas;            // Number of elements in the alpha array
        int NumNums;              // Number of elements in the numeric array
        Array1D_string AlphArray; // character string data
        Array1D<Real64> NumArray; // numeric data
        int IOStat;               // IO Status when calling get input subroutine
        auto &ort(state.dataOutRptTab);

        inputProcessor->getObjectDefMaxArgs(state, CurrentModuleObject, NumParams, NumAlphas, NumNums);
        AlphArray.allocate(NumAlphas);
        NumArray.dimension(NumNums, 0.0);

        NumTabularStyle = inputProcessor->getNumObjectsFound(state, CurrentModuleObject);

        if (NumTabularStyle == 0) {
            AlphArray(1) = "COMMA";
            ort->numStyles = 1;
            ort->TableStyle(1) = iTableStyle::Comma;
            ort->del(1) = CharComma; // comma
            ort->unitsStyle = iUnitsStyle::None;
        } else if (NumTabularStyle == 1) {
            inputProcessor->getObjectItem(state,
                                          CurrentModuleObject,
                                          1,
                                          AlphArray,
                                          NumAlphas,
                                          NumArray,
                                          NumNums,
                                          IOStat,
                                          lNumericFieldBlanks,
                                          lAlphaFieldBlanks,
                                          cAlphaFieldNames,
                                          cNumericFieldNames);
            // ColumnSeparator
            if (UtilityRoutines::SameString(AlphArray(1), "Comma")) {
                ort->numStyles = 1;
                ort->TableStyle(1) = iTableStyle::Comma;
                ort->del(1) = CharComma; // comma
            } else if (UtilityRoutines::SameString(AlphArray(1), "Tab")) {
                ort->numStyles = 1;
                ort->TableStyle(1) = iTableStyle::Tab;
                ort->del(1) = CharTab; // tab
            } else if (UtilityRoutines::SameString(AlphArray(1), "Fixed")) {
                ort->numStyles = 1;
                ort->TableStyle(1) = iTableStyle::Fixed;
                ort->del(1) = CharSpace; // space
            } else if (UtilityRoutines::SameString(AlphArray(1), "HTML")) {
                ort->numStyles = 1;
                ort->TableStyle(1) = iTableStyle::HTML;
                ort->del(1) = CharSpace; // space - this is not used much for HTML output
            } else if (UtilityRoutines::SameString(AlphArray(1), "XML")) {
                ort->numStyles = 1;
                ort->TableStyle(1) = iTableStyle::XML;
                ort->del(1) = CharSpace; // space - this is not used much for XML output
            } else if (UtilityRoutines::SameString(AlphArray(1), "CommaAndHTML")) {
                ort->numStyles = 2;
                ort->TableStyle(1) = iTableStyle::Comma;
                ort->del(1) = CharComma; // comma
                ort->TableStyle(2) = iTableStyle::HTML;
                ort->del(2) = CharSpace; // space - this is not used much for HTML output
            } else if (UtilityRoutines::SameString(AlphArray(1), "CommaAndXML")) {
                ort->numStyles = 2;
                ort->TableStyle(1) = iTableStyle::Comma;
                ort->del(1) = CharComma; // comma
                ort->TableStyle(2) = iTableStyle::XML;
                ort->del(2) = CharSpace; // space - this is not used much for XML output
            } else if (UtilityRoutines::SameString(AlphArray(1), "TabAndHTML")) {
                ort->numStyles = 2;
                ort->TableStyle(1) = iTableStyle::Tab;
                ort->del(1) = CharTab; // tab
                ort->TableStyle(2) = iTableStyle::HTML;
                ort->del(2) = CharSpace; // space - this is not used much for HTML output
            } else if (UtilityRoutines::SameString(AlphArray(1), "XMLandHTML")) {
                ort->numStyles = 2;
                ort->TableStyle(1) = iTableStyle::XML;
                ort->del(1) = CharSpace; // space - this is not used much for XML output
                ort->TableStyle(2) = iTableStyle::HTML;
                ort->del(2) = CharSpace; // space - this is not used much for HTML output
            } else if (UtilityRoutines::SameString(AlphArray(1), "All")) {
                ort->numStyles = 5;
                ort->TableStyle(1) = iTableStyle::Comma;
                ort->del(1) = CharComma; // comma
                ort->TableStyle(2) = iTableStyle::Tab;
                ort->del(2) = CharTab; // tab
                ort->TableStyle(3) = iTableStyle::Fixed;
                ort->del(3) = CharSpace; // space
                ort->TableStyle(4) = iTableStyle::HTML;
                ort->del(4) = CharSpace; // space - this is not used much for HTML output
                ort->TableStyle(5) = iTableStyle::XML;
                ort->del(5) = CharSpace; // space - this is not used much for XML output
            } else {
                ShowWarningError(state, CurrentModuleObject + ": Invalid " + cAlphaFieldNames(1) + "=\"" + AlphArray(1) + "\". Commas will be used.");
                ort->numStyles = 1;
                ort->TableStyle(1) = iTableStyle::Comma;
                ort->del(1) = CharComma; // comma
                AlphArray(1) = "COMMA";
            }
            // MonthlyUnitConversion
            if (NumAlphas >= 2) {
                ort->unitsStyle = SetUnitsStyleFromString(AlphArray(2));
                if (ort->unitsStyle == iUnitsStyle::NotFound) {
                    ShowWarningError(state, CurrentModuleObject + ": Invalid " + cAlphaFieldNames(2) + "=\"" + AlphArray(2) +
                                     "\". No unit conversion will be performed. Normal SI units will be shown.");
                }
            } else {
                ort->unitsStyle = iUnitsStyle::None;
                AlphArray(2) = "None";
            }
        } else if (NumTabularStyle > 1) {
            ShowWarningError(state, CurrentModuleObject + ": Only one instance of this object is allowed. Commas will be used.");
            ort->TableStyle = iTableStyle::Comma;
            ort->del = std::string(1, CharComma); // comma
            AlphArray(1) = "COMMA";
            ort->unitsStyle = iUnitsStyle::None;
            AlphArray(2) = "None";
        }

        if (ort->WriteTabularFiles) {
            print(state.files.eio, "! <Tabular Report>,Style,Unit Conversion\n");
            if (AlphArray(1) != "HTML") {
                ConvertCaseToLower(AlphArray(1), AlphArray(2));
                AlphArray(1).erase(1);
                AlphArray(1) += AlphArray(2).substr(1);
            }
            print(state.files.eio, "Tabular Report,{},{}\n", AlphArray(1), AlphArray(2));
        }
    }

    iUnitsStyle SetUnitsStyleFromString(std::string const &unitStringIn)
    {
        iUnitsStyle unitsStyleReturn;
        if (UtilityRoutines::SameString(unitStringIn, "None")) {
            unitsStyleReturn = iUnitsStyle::None;
        } else if (UtilityRoutines::SameString(unitStringIn, "JTOKWH")) {
            unitsStyleReturn = iUnitsStyle::JtoKWH;
        } else if (UtilityRoutines::SameString(unitStringIn, "JTOMJ")) {
            unitsStyleReturn = iUnitsStyle::JtoMJ;
        } else if (UtilityRoutines::SameString(unitStringIn, "JTOGJ")) {
            unitsStyleReturn = iUnitsStyle::JtoGJ;
        } else if (UtilityRoutines::SameString(unitStringIn, "INCHPOUND")) {
            unitsStyleReturn = iUnitsStyle::InchPound;
        } else {
            unitsStyleReturn = iUnitsStyle::NotFound;
        }
        return unitsStyleReturn;
    }

    void GetInputOutputTableSummaryReports(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   November 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   This routine flags if any of the predefined reports
        //   are requested by the user

        // METHODOLOGY EMPLOYED:
        //   Uses get input structure similar to other objects

        // REFERENCES:
        // na

        // Using/Aliasing
        using namespace DataIPShortCuts;
        using DataStringGlobals::CharComma;
        using DataStringGlobals::CharSpace;
        using DataStringGlobals::CharTab;

        // SUBROUTINE PARAMETER DEFINITIONS:
        static std::string const CurrentModuleObject("Output:Table:SummaryReports");

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:

        int NumTabularPredefined;
        int NumParams;
        int NumAlphas; // Number of elements in the alpha array
        int NumNums;   // Number of elements in the numeric array
        Array1D_string AlphArray;
        Array1D<Real64> NumArray;
        int IOStat; // IO Status when calling get input subroutine
        int iReport;
        std::string meterName;
        int meterNumber;
        int iResource;
        int kEndUseSub;
        int jReport;
        bool nameFound;
        bool ErrorsFound;
        auto &ort(state.dataOutRptTab);

        if (!(state.files.outputControl.tabular || state.files.outputControl.sqlite)) {
            ort->WriteTabularFiles = false;
            return;
        }

        ErrorsFound = false;
        NumTabularPredefined = inputProcessor->getNumObjectsFound(state, CurrentModuleObject);
        if (NumTabularPredefined == 1) {
            // find out how many fields since the object is extensible
            inputProcessor->getObjectDefMaxArgs(state, CurrentModuleObject, NumParams, NumAlphas, NumNums);
            // allocate the temporary arrays for the call to get the filed
            AlphArray.allocate(NumAlphas);
            // don't really need the NumArray since not expecting any numbers but the call requires it
            NumArray.dimension(NumNums, 0.0);
            // get the object
            inputProcessor->getObjectItem(state, CurrentModuleObject, 1, AlphArray, NumAlphas, NumArray, NumNums, IOStat);
            // default all report flags to false (do not get produced)
            ort->displayTabularBEPS = false;
            // initialize the names of the predefined monthly report titles
            InitializePredefinedMonthlyTitles(state);
            // loop through the fields looking for matching report titles
            for (iReport = 1; iReport <= NumAlphas; ++iReport) {
                nameFound = false;
                if (AlphArray(iReport).empty()) {
                    ShowFatalError(state, "Blank report name in Output:Table:SummaryReports");
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "AnnualBuildingUtilityPerformanceSummary")) {
                    ort->displayTabularBEPS = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "ComponentCostEconomicsSummary")) {
                    ort->displayTabularCompCosts = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "InputVerificationandResultsSummary")) {
                    ort->displayTabularVeriSum = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "ComponentSizingSummary")) {
                    ort->displayComponentSizing = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "SurfaceShadowingSummary")) {
                    ort->displaySurfaceShadowing = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "DemandEndUseComponentsSummary")) {
                    ort->displayDemandEndUse = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "AdaptiveComfortSummary")) {
                    ort->displayAdaptiveComfort = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "SourceEnergyEndUseComponentsSummary")) {
                    ort->displaySourceEnergyEndUseSummary = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "ZoneComponentLoadSummary")) {
                    ort->displayZoneComponentLoadSummary = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "AirLoopComponentLoadSummary")) {
                    ort->displayAirLoopComponentLoadSummary = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "FacilityComponentLoadSummary")) {
                    ort->displayFacilityComponentLoadSummary = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "LEEDSummary")) {
                    ort->displayLEEDSummary = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "LifeCycleCostReport")) {
                    ort->displayLifeCycleCostReport = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "TariffReport")) {
                    ort->displayTariffReport = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "EconomicResultSummary")) {
                    ort->displayEconomicResultSummary = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "HeatEmissionsSummary")) {
                    ort->displayHeatEmissionsSummary = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "ThermalResilienceSummary")) {
                    ort->displayThermalResilienceSummary = true;
                    ort->displayThermalResilienceSummaryExplicitly = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "CO2ResilienceSummary")) {
                    ort->displayCO2ResilienceSummary = true;
                    ort->displayCO2ResilienceSummaryExplicitly = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "VisualResilienceSummary")) {
                    ort->displayVisualResilienceSummary = true;
                    ort->displayVisualResilienceSummaryExplicitly = true;
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "EnergyMeters")) {
                    ort->WriteTabularFiles = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "InitializationSummary")) {
                    ort->WriteTabularFiles = true;
                    ort->displayEioSummary = true;
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "AllSummary")) {
                    ort->WriteTabularFiles = true;
                    ort->displayTabularBEPS = true;
                    ort->displayTabularVeriSum = true;
                    ort->displayTabularCompCosts = true;
                    ort->displaySurfaceShadowing = true;
                    ort->displayComponentSizing = true;
                    ort->displayDemandEndUse = true;
                    ort->displayAdaptiveComfort = true;
                    ort->displaySourceEnergyEndUseSummary = true;
                    ort->displayLifeCycleCostReport = true;
                    ort->displayTariffReport = true;
                    ort->displayEconomicResultSummary = true;
                    ort->displayEioSummary = true;
                    ort->displayLEEDSummary = true;
                    ort->displayHeatEmissionsSummary = true;
                    ort->displayThermalResilienceSummary = true;
                    ort->displayCO2ResilienceSummary = true;
                    ort->displayVisualResilienceSummary = true;
                    nameFound = true;
                    for (jReport = 1; jReport <= state.dataOutRptPredefined->numReportName; ++jReport) {
                        state.dataOutRptPredefined->reportName(jReport).show = true;
                    }
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "AllSummaryAndSizingPeriod")) {
                    ort->WriteTabularFiles = true;
                    ort->displayTabularBEPS = true;
                    ort->displayTabularVeriSum = true;
                    ort->displayTabularCompCosts = true;
                    ort->displaySurfaceShadowing = true;
                    ort->displayComponentSizing = true;
                    ort->displayDemandEndUse = true;
                    ort->displayAdaptiveComfort = true;
                    ort->displaySourceEnergyEndUseSummary = true;
                    ort->displayLifeCycleCostReport = true;
                    ort->displayTariffReport = true;
                    ort->displayEconomicResultSummary = true;
                    ort->displayEioSummary = true;
                    ort->displayLEEDSummary = true;
                    ort->displayHeatEmissionsSummary = true;
                    ort->displayThermalResilienceSummary = true;
                    ort->displayCO2ResilienceSummary = true;
                    ort->displayVisualResilienceSummary = true;
                    nameFound = true;
                    for (jReport = 1; jReport <= state.dataOutRptPredefined->numReportName; ++jReport) {
                        state.dataOutRptPredefined->reportName(jReport).show = true;
                    }
                    // the sizing period reports
                    ort->displayZoneComponentLoadSummary = true;
                    ort->displayAirLoopComponentLoadSummary = true;
                    ort->displayFacilityComponentLoadSummary = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "AllMonthly")) {
                    ort->WriteTabularFiles = true;
                    for (jReport = 1; jReport <= numNamedMonthly; ++jReport) {
                        ort->namedMonthly(jReport).show = true;
                    }
                    nameFound = true;
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "AllSummaryAndMonthly")) {
                    ort->WriteTabularFiles = true;
                    ort->displayTabularBEPS = true;
                    ort->displayTabularVeriSum = true;
                    ort->displayTabularCompCosts = true;
                    ort->displaySurfaceShadowing = true;
                    ort->displayComponentSizing = true;
                    ort->displayDemandEndUse = true;
                    ort->displayAdaptiveComfort = true;
                    ort->displaySourceEnergyEndUseSummary = true;
                    ort->displayLifeCycleCostReport = true;
                    ort->displayTariffReport = true;
                    ort->displayEconomicResultSummary = true;
                    ort->displayEioSummary = true;
                    ort->displayLEEDSummary = true;
                    ort->displayHeatEmissionsSummary = true;
                    ort->displayThermalResilienceSummary = true;
                    ort->displayCO2ResilienceSummary = true;
                    ort->displayVisualResilienceSummary = true;
                    nameFound = true;
                    for (jReport = 1; jReport <= state.dataOutRptPredefined->numReportName; ++jReport) {
                        state.dataOutRptPredefined->reportName(jReport).show = true;
                    }
                    for (jReport = 1; jReport <= numNamedMonthly; ++jReport) {
                        ort->namedMonthly(jReport).show = true;
                    }
                } else if (UtilityRoutines::SameString(AlphArray(iReport), "AllSummaryMonthlyAndSizingPeriod")) {
                    ort->WriteTabularFiles = true;
                    ort->displayTabularBEPS = true;
                    ort->displayTabularVeriSum = true;
                    ort->displayTabularCompCosts = true;
                    ort->displaySurfaceShadowing = true;
                    ort->displayComponentSizing = true;
                    ort->displayDemandEndUse = true;
                    ort->displayAdaptiveComfort = true;
                    ort->displaySourceEnergyEndUseSummary = true;
                    ort->displayLifeCycleCostReport = true;
                    ort->displayTariffReport = true;
                    ort->displayEconomicResultSummary = true;
                    ort->displayEioSummary = true;
                    ort->displayLEEDSummary = true;
                    ort->displayHeatEmissionsSummary = true;
                    ort->displayThermalResilienceSummary = true;
                    ort->displayCO2ResilienceSummary = true;
                    ort->displayVisualResilienceSummary = true;
                    nameFound = true;
                    for (jReport = 1; jReport <= state.dataOutRptPredefined->numReportName; ++jReport) {
                        state.dataOutRptPredefined->reportName(jReport).show = true;
                    }
                    for (jReport = 1; jReport <= numNamedMonthly; ++jReport) {
                        ort->namedMonthly(jReport).show = true;
                    }
                    // the sizing period reports
                    ort->displayZoneComponentLoadSummary = true;
                    ort->displayAirLoopComponentLoadSummary = true;
                    ort->displayFacilityComponentLoadSummary = true;
                }
                // check the reports that are predefined and are created by OutputReportPredefined
                for (jReport = 1; jReport <= state.dataOutRptPredefined->numReportName; ++jReport) {
                    if (UtilityRoutines::SameString(AlphArray(iReport), state.dataOutRptPredefined->reportName(jReport).name)) {
                        ort->WriteTabularFiles = true;
                        state.dataOutRptPredefined->reportName(jReport).show = true;
                        nameFound = true;
                    }
                    if (UtilityRoutines::SameString(AlphArray(iReport), state.dataOutRptPredefined->reportName(jReport).abrev)) {
                        ort->WriteTabularFiles = true;
                        state.dataOutRptPredefined->reportName(jReport).show = true;
                        nameFound = true;
                    }
                }
                // check if the predefined monthly reports are used
                for (jReport = 1; jReport <= numNamedMonthly; ++jReport) {
                    if (UtilityRoutines::SameString(AlphArray(iReport), ort->namedMonthly(jReport).title)) {
                        ort->namedMonthly(jReport).show = true;
                        ort->WriteTabularFiles = true;
                        nameFound = true;
                    }
                }
                if (!nameFound) {
                    ShowSevereError(
                        state,
                        format(
                            "{} Field[{}]=\"{}\", invalid report name -- will not be reported.", CurrentModuleObject, iReport, AlphArray(iReport)));
                    //      ErrorsFound=.TRUE.
                }
            }
            CreatePredefinedMonthlyReports(state);
        } else if (NumTabularPredefined > 1) {
            ShowSevereError(state, CurrentModuleObject + ": Only one instance of this object is allowed.");
            ErrorsFound = true;
        }
        if (ErrorsFound) {
            ShowFatalError(state, CurrentModuleObject + ": Preceding errors cause termination.");
        }
        // if the BEPS report has been called for than initialize its arrays
        if (ort->displayTabularBEPS || ort->displayDemandEndUse || ort->displaySourceEnergyEndUseSummary || ort->displayLEEDSummary) {
            // initialize the resource type names
            ort->resourceTypeNames(1) = "Electricity";
            ort->resourceTypeNames(2) = "NaturalGas";
            ort->resourceTypeNames(3) = "DistrictCooling";
            ort->resourceTypeNames(4) = "DistrictHeating";
            ort->resourceTypeNames(5) = "Steam";
            ort->resourceTypeNames(6) = "Gasoline";
            ort->resourceTypeNames(7) = "Water";
            ort->resourceTypeNames(8) = "Diesel";
            ort->resourceTypeNames(9) = "Coal";
            ort->resourceTypeNames(10) = "FuelOilNo1";
            ort->resourceTypeNames(11) = "FuelOilNo2";
            ort->resourceTypeNames(12) = "Propane";
            ort->resourceTypeNames(13) = "OtherFuel1";
            ort->resourceTypeNames(14) = "OtherFuel2";

            ort->sourceTypeNames(1) = "Electricity";
            ort->sourceTypeNames(2) = "NaturalGas";
            ort->sourceTypeNames(3) = "Gasoline";
            ort->sourceTypeNames(4) = "Diesel";
            ort->sourceTypeNames(5) = "Coal";
            ort->sourceTypeNames(6) = "FuelOilNo1";
            ort->sourceTypeNames(7) = "FuelOilNo2";
            ort->sourceTypeNames(8) = "Propane";
            ort->sourceTypeNames(9) = "PurchasedElectric";
            ort->sourceTypeNames(10) = "SoldElectric";
            ort->sourceTypeNames(11) = "OtherFuel1";
            ort->sourceTypeNames(12) = "OtherFuel2";

            // initialize the end use names
            ort->endUseNames(DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Heating)) = "Heating";
            ort->endUseNames(DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Cooling)) = "Cooling";
            ort->endUseNames(DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::InteriorLights)) = "InteriorLights";
            ort->endUseNames(DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::ExteriorLights)) = "ExteriorLights";
            ort->endUseNames(DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::InteriorEquipment)) = "InteriorEquipment";
            ort->endUseNames(DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::ExteriorEquipment)) = "ExteriorEquipment";
            ort->endUseNames(DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Fans)) = "Fans";
            ort->endUseNames(DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Pumps)) = "Pumps";
            ort->endUseNames(DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::HeatRejection)) = "HeatRejection";
            ort->endUseNames(DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Humidification)) = "Humidifier";
            ort->endUseNames(DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::HeatRecovery)) = "HeatRecovery";
            ort->endUseNames(DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::WaterSystem)) = "WaterSystems";
            ort->endUseNames(DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Refrigeration)) = "Refrigeration";
            ort->endUseNames(DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Cogeneration)) = "Cogeneration";

            // End use subs must be dynamically allocated to accomodate the end use with the most subcategories
            ort->meterNumEndUseSubBEPS.allocate(state.dataOutputProcessor->MaxNumSubcategories, DataGlobalConstants::iEndUse.size(), numResourceTypes);
            ort->meterNumEndUseSubBEPS = 0;

            // loop through all of the resources and end uses and sub end uses for the entire facility
            for (iResource = 1; iResource <= numResourceTypes; ++iResource) {
                meterName = ort->resourceTypeNames(iResource) + ":FACILITY";
                meterNumber = GetMeterIndex(state, meterName);
                ort->meterNumTotalsBEPS(iResource) = meterNumber;

                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    meterName = ort->endUseNames(jEndUse) + ':' + ort->resourceTypeNames(iResource); //// ':FACILITY'
                    meterNumber = GetMeterIndex(state, meterName);
                    ort->meterNumEndUseBEPS(iResource, jEndUse) = meterNumber;

                    for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                        meterName =
                            state.dataOutputProcessor->EndUseCategory(jEndUse).SubcategoryName(kEndUseSub) + ':' + ort->endUseNames(jEndUse) + ':' + ort->resourceTypeNames(iResource);
                        meterNumber = GetMeterIndex(state, meterName);
                        ort->meterNumEndUseSubBEPS(kEndUseSub, jEndUse, iResource) = meterNumber;
                    }
                }
            }

            for (iResource = 1; iResource <= numSourceTypes; ++iResource) {
                meterNumber = GetMeterIndex(state, ort->sourceTypeNames(iResource) + "Emissions:Source");
                ort->meterNumTotalsSource(iResource) = meterNumber;
            }

            // initialize the gathering arrays to zero
            ort->gatherTotalsBEPS = 0.0;
            ort->gatherTotalsBySourceBEPS = 0.0;
            ort->gatherTotalsSource = 0.0;
            ort->gatherTotalsBySource = 0.0;
            ort->gatherEndUseBEPS = 0.0;
            ort->gatherEndUseBySourceBEPS = 0.0;
            // End use subs must be dynamically allocated to accommodate the end use with the most subcategories
            ort->gatherEndUseSubBEPS.allocate(state.dataOutputProcessor->MaxNumSubcategories, DataGlobalConstants::iEndUse.size(), numResourceTypes);
            ort->gatherEndUseSubBEPS = 0.0;
            ort->gatherDemandEndUseSub.allocate(state.dataOutputProcessor->MaxNumSubcategories, DataGlobalConstants::iEndUse.size(), numResourceTypes);
            ort->gatherDemandEndUseSub = 0.0;
            ort->gatherDemandIndEndUseSub.allocate(state.dataOutputProcessor->MaxNumSubcategories, DataGlobalConstants::iEndUse.size(), numResourceTypes);
            ort->gatherDemandIndEndUseSub = 0.0;

            // get meter numbers for other meters relating to electric load components
            ort->meterNumPowerFuelFireGen = GetMeterIndex(state, "Cogeneration:ElectricityProduced");
            ort->meterNumPowerPV = GetMeterIndex(state, "Photovoltaic:ElectricityProduced");
            ort->meterNumPowerWind = GetMeterIndex(state, "WindTurbine:ElectricityProduced");
            ort->meterNumPowerHTGeothermal = GetMeterIndex(state, "HTGeothermal:ElectricityProduced");
            ort->meterNumElecStorage = GetMeterIndex(state, "ElectricStorage:ElectricityProduced");
            ort->meterNumPowerConversion = GetMeterIndex(state, "PowerConversion:ElectricityProduced");
            ort->meterNumElecProduced = GetMeterIndex(state, "ElectricityProduced:Facility");
            ort->meterNumElecPurchased = GetMeterIndex(state, "ElectricityPurchased:Facility");
            ort->meterNumElecSurplusSold = GetMeterIndex(state, "ElectricitySurplusSold:Facility");
            // if no ElectricityPurchased:Facility meter is defined then no electric load center
            // was created by the user and no power generation will occur in the plant. The amount
            // purchased would be the total end use.
            if (ort->meterNumElecPurchased == 0) {
                ort->meterNumElecPurchased = GetMeterIndex(state, "Electricity:Facility");
            }

            // initialize the gathering variables for the electric load components
            ort->gatherPowerFuelFireGen = 0.0;
            ort->gatherPowerPV = 0.0;
            ort->gatherPowerWind = 0.0;
            ort->gatherPowerHTGeothermal = 0.0;
            ort->gatherElecProduced = 0.0;
            ort->gatherElecPurchased = 0.0;
            ort->gatherElecSurplusSold = 0.0;
            ort->gatherElecStorage = 0.0;
            ort->gatherPowerConversion = 0.0;

            // get meter numbers for onsite thermal components on BEPS report
            ort->meterNumWaterHeatRecovery = GetMeterIndex(state, "HeatRecovery:EnergyTransfer");
            ort->meterNumAirHeatRecoveryCool = GetMeterIndex(state, "HeatRecoveryForCooling:EnergyTransfer");
            ort->meterNumAirHeatRecoveryHeat = GetMeterIndex(state, "HeatRecoveryForHeating:EnergyTransfer");
            ort->meterNumHeatHTGeothermal = GetMeterIndex(state, "HTGeothermal:HeatProduced");
            ort->meterNumHeatSolarWater = GetMeterIndex(state, "SolarWater:Facility");
            ort->meterNumHeatSolarAir = GetMeterIndex(state, "HeatProduced:SolarAir");
            // initialize the gathering variables for onsite thermal components on BEPS report
            ort->gatherWaterHeatRecovery = 0.0;
            ort->gatherAirHeatRecoveryCool = 0.0;
            ort->gatherAirHeatRecoveryHeat = 0.0;
            ort->gatherHeatHTGeothermal = 0.0;
            ort->gatherHeatSolarWater = 0.0;
            ort->gatherHeatSolarAir = 0.0;

            // get meter numbers for water components on BEPS report
            ort->meterNumRainWater = GetMeterIndex(state, "Rainwater:OnSiteWater");
            ort->meterNumCondensate = GetMeterIndex(state, "Condensate:OnSiteWater");
            ort->meterNumGroundwater = GetMeterIndex(state, "Wellwater:OnSiteWater");
            ort->meterNumMains = GetMeterIndex(state, "MainsWater:Facility");
            ort->meterNumWaterEndUseTotal = GetMeterIndex(state, "Water:Facility");

            // initialize the gathering variables for water components on BEPS report
            ort->gatherRainWater = 0.0;
            ort->gatherCondensate = 0.0;
            ort->gatherWellwater = 0.0;
            ort->gatherMains = 0.0;
            ort->gatherWaterEndUseTotal = 0.0;
        }
    }

    bool isCompLoadRepReq(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   November 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Determine if the ZoneComponentLoadSummary or
        //   ZoneComponentLoadDetail reports are requested.

        // METHODOLOGY EMPLOYED:
        //   Uses get input structure similar to other objects

        // REFERENCES:
        // na

        // USE STATEMENTS:
        // na

        // Return value
        bool isCompLoadRepReq;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        static std::string const CurrentModuleObject("Output:Table:SummaryReports");

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int NumTabularPredefined;
        int NumParams;
        int NumAlphas; // Number of elements in the alpha array
        int NumNums;   // Number of elements in the numeric array
        Array1D_string AlphArray;
        Array1D<Real64> NumArray;
        int IOStat; // IO Status when calling get input subroutine
        int iReport;
        bool isFound;

        isFound = false;
        NumTabularPredefined = inputProcessor->getNumObjectsFound(state, CurrentModuleObject);
        if (NumTabularPredefined == 1) {
            // find out how many fields since the object is extensible
            inputProcessor->getObjectDefMaxArgs(state, CurrentModuleObject, NumParams, NumAlphas, NumNums);
            // allocate the temporary arrays for the call to get the filed
            AlphArray.allocate(NumAlphas);
            // don't really need the NumArray since not expecting any numbers but the call requires it
            NumArray.dimension(NumNums, 0.0);
            // get the object
            inputProcessor->getObjectItem(state, CurrentModuleObject, 1, AlphArray, NumAlphas, NumArray, NumNums, IOStat);
            // loop through the fields looking for matching report titles
            for (iReport = 1; iReport <= NumAlphas; ++iReport) {
                if (UtilityRoutines::SameString(AlphArray(iReport), "ZoneComponentLoadSummary")) {
                    isFound = true;
                }
                if (UtilityRoutines::SameString(AlphArray(iReport), "AirLoopComponentLoadSummary")) {
                    isFound = true;
                }
                if (UtilityRoutines::SameString(AlphArray(iReport), "FacilityComponentLoadSummary")) {
                    isFound = true;
                }
                if (UtilityRoutines::SameString(AlphArray(iReport), "AllSummaryAndSizingPeriod")) {
                    isFound = true;
                }
                if (UtilityRoutines::SameString(AlphArray(iReport), "AllSummaryMonthlyAndSizingPeriod")) {
                    isFound = true;
                }
            }
        }
        isCompLoadRepReq = isFound; // return true if either report was found
        return isCompLoadRepReq;
    }

    bool hasSizingPeriodsDays(EnergyPlusData &state)
    {
        int sizePerDesDays = inputProcessor->getNumObjectsFound(state, "SizingPeriod:DesignDay");
        int sizePerWeathFileDays = inputProcessor->getNumObjectsFound(state, "SizingPeriod:WeatherFileDays");
        return ((sizePerDesDays + sizePerWeathFileDays) > 0);
    }

    void InitializePredefinedMonthlyTitles(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   September 2008
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Initialize the NamedMonthly array for the titles
        //   of the monthly predefined reports

        // METHODOLOGY EMPLOYED:
        // REFERENCES:
        // na

        // Using/Aliasing
        using namespace DataOutputs;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int xcount;
        auto &ort(state.dataOutRptTab);

        ort->namedMonthly.allocate(numNamedMonthly);
        ort->namedMonthly(1).title = "ZoneCoolingSummaryMonthly";
        ort->namedMonthly(2).title = "ZoneHeatingSummaryMonthly";
        ort->namedMonthly(3).title = "ZoneElectricSummaryMonthly";
        ort->namedMonthly(4).title = "SpaceGainsMonthly";
        ort->namedMonthly(5).title = "PeakSpaceGainsMonthly";
        ort->namedMonthly(6).title = "SpaceGainComponentsAtCoolingPeakMonthly";
        ort->namedMonthly(7).title = "EnergyConsumptionElectricityNaturalGasMonthly";
        ort->namedMonthly(8).title = "EnergyConsumptionElectricityGeneratedPropaneMonthly";
        ort->namedMonthly(9).title = "EnergyConsumptionDieselFuelOilMonthly";
        ort->namedMonthly(10).title = "EnergyConsumptionDistrictHeatingCoolingMonthly";
        ort->namedMonthly(11).title = "EnergyConsumptionCoalGasolineMonthly";
        ort->namedMonthly(12).title = "EnergyConsumptionOtherFuelsMonthly";
        ort->namedMonthly(13).title = "EndUseEnergyConsumptionElectricityMonthly";
        ort->namedMonthly(14).title = "EndUseEnergyConsumptionNaturalGasMonthly";
        ort->namedMonthly(15).title = "EndUseEnergyConsumptionDieselMonthly";
        ort->namedMonthly(16).title = "EndUseEnergyConsumptionFuelOilMonthly";
        ort->namedMonthly(17).title = "EndUseEnergyConsumptionCoalMonthly";
        ort->namedMonthly(18).title = "EndUseEnergyConsumptionPropaneMonthly";
        ort->namedMonthly(19).title = "EndUseEnergyConsumptionGasolineMonthly";
        ort->namedMonthly(20).title = "EndUseEnergyConsumptionOtherFuelsMonthly";
        ort->namedMonthly(21).title = "PeakEnergyEndUseElectricityPart1Monthly";
        ort->namedMonthly(22).title = "PeakEnergyEndUseElectricityPart2Monthly";
        ort->namedMonthly(23).title = "ElectricComponentsOfPeakDemandMonthly";
        ort->namedMonthly(24).title = "PeakEnergyEndUseNaturalGasMonthly";
        ort->namedMonthly(25).title = "PeakEnergyEndUseDieselMonthly";
        ort->namedMonthly(26).title = "PeakEnergyEndUseFuelOilMonthly";
        ort->namedMonthly(27).title = "PeakEnergyEndUseCoalMonthly";
        ort->namedMonthly(28).title = "PeakEnergyEndUsePropaneMonthly";
        ort->namedMonthly(29).title = "PeakEnergyEndUseGasolineMonthly";
        ort->namedMonthly(30).title = "PeakEnergyEndUseOtherFuelsMonthly";
        ort->namedMonthly(31).title = "SetpointsNotMetWithTemperaturesMonthly";
        ort->namedMonthly(32).title = "ComfortReportSimple55Monthly";
        ort->namedMonthly(33).title = "UnglazedTranspiredSolarCollectorSummaryMonthly";
        ort->namedMonthly(34).title = "OccupantComfortDataSummaryMonthly";
        ort->namedMonthly(35).title = "ChillerReportMonthly";
        ort->namedMonthly(36).title = "TowerReportMonthly";
        ort->namedMonthly(37).title = "BoilerReportMonthly";
        ort->namedMonthly(38).title = "DXReportMonthly";
        ort->namedMonthly(39).title = "WindowReportMonthly";
        ort->namedMonthly(40).title = "WindowEnergyReportMonthly";
        ort->namedMonthly(41).title = "WindowZoneSummaryMonthly";
        ort->namedMonthly(42).title = "WindowEnergyZoneSummaryMonthly";
        ort->namedMonthly(43).title = "AverageOutdoorConditionsMonthly";
        ort->namedMonthly(44).title = "OutdoorConditionsMaximumDryBulbMonthly";
        ort->namedMonthly(45).title = "OutdoorConditionsMinimumDryBulbMonthly";
        ort->namedMonthly(46).title = "OutdoorConditionsMaximumWetBulbMonthly";
        ort->namedMonthly(47).title = "OutdoorConditionsMaximumDewPointMonthly";
        ort->namedMonthly(48).title = "OutdoorGroundConditionsMonthly";
        ort->namedMonthly(49).title = "WindowACReportMonthly";
        ort->namedMonthly(50).title = "WaterHeaterReportMonthly";
        ort->namedMonthly(51).title = "GeneratorReportMonthly";
        ort->namedMonthly(52).title = "DaylightingReportMonthly";
        ort->namedMonthly(53).title = "CoilReportMonthly";
        ort->namedMonthly(54).title = "PlantLoopDemandReportMonthly";
        ort->namedMonthly(55).title = "FanReportMonthly";
        ort->namedMonthly(56).title = "PumpReportMonthly";
        ort->namedMonthly(57).title = "CondLoopDemandReportMonthly";
        ort->namedMonthly(58).title = "ZoneTemperatureOscillationReportMonthly";
        ort->namedMonthly(59).title = "AirLoopSystemEnergyAndWaterUseMonthly";
        ort->namedMonthly(60).title = "AirLoopSystemComponentLoadsMonthly";
        ort->namedMonthly(61).title = "AirLoopSystemComponentEnergyUseMonthly";
        ort->namedMonthly(62).title = "MechanicalVentilationLoadsMonthly";
        ort->namedMonthly(63).title = "HeatEmissionsReportMonthly";

        if (numNamedMonthly != NumMonthlyReports) {
            ShowFatalError(state,
                           format("InitializePredefinedMonthlyTitles: Number of Monthly Reports in OutputReportTabular=[{}] does not match number in "
                                  "DataOutputs=[{}].",
                                  numNamedMonthly,
                                  NumMonthlyReports));
        } else {
            for (xcount = 1; xcount <= numNamedMonthly; ++xcount) {
                if (!UtilityRoutines::SameString(MonthlyNamedReports(xcount), ort->namedMonthly(xcount).title)) {
                    ShowSevereError(state,
                        "InitializePredefinedMonthlyTitles: Monthly Report Titles in OutputReportTabular do not match titles in DataOutput.");
                    ShowContinueError(state, format("first mismatch at ORT [{}] =\"{}\".", numNamedMonthly, ort->namedMonthly(xcount).title));
                    ShowContinueError(state, "same location in DO =\"" + MonthlyNamedReports(xcount) + "\".");
                    ShowFatalError(state, "Preceding condition causes termination.");
                }
            }
        }
    }

    void CreatePredefinedMonthlyReports(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   September 2008
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   For any predefined monthly reports that have been
        //   called out, define the individual columns.

        // METHODOLOGY EMPLOYED:
        // REFERENCES:
        // na

        // USE STATEMENTS:

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int curReport;
        auto &ort(state.dataOutRptTab);

        // ----------------------------------------------------------------------------------------
        // If any variable are added to these reports they also need to be added to the
        // AddVariablesForMonthlyReport routine in InputProcessor.
        // ----------------------------------------------------------------------------------------

        if (ort->namedMonthly(1).show) {
            curReport = AddMonthlyReport(state, "ZoneCoolingSummaryMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Zone Air System Sensible Cooling Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Air System Sensible Cooling Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Drybulb Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Wetbulb Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Zone Total Internal Latent Gain Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Total Internal Latent Gain Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Drybulb Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Wetbulb Temperature", "", iAggType::ValueWhenMaxMin);
        }
        if (ort->namedMonthly(2).show) {
            curReport = AddMonthlyReport(state, "ZoneHeatingSummaryMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Zone Air System Sensible Heating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Air System Sensible Heating Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Drybulb Temperature", "", iAggType::ValueWhenMaxMin);
        }
        if (ort->namedMonthly(3).show) {
            curReport = AddMonthlyReport(state, "ZoneElectricSummaryMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Zone Lights Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Lights Electricity Energy", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Zone Electric Equipment Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Electric Equipment Electricity Energy", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(4).show) {
            curReport = AddMonthlyReport(state, "SpaceGainsMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Zone People Total Heating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Lights Total Heating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Electric Equipment Total Heating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Gas Equipment Total Heating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Hot Water Equipment Total Heating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Steam Equipment Total Heating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Other Equipment Total Heating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Infiltration Sensible Heat Gain Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Infiltration Sensible Heat Loss Energy", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(5).show) {
            curReport = AddMonthlyReport(state, "PeakSpaceGainsMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Zone People Total Heating Energy", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Zone Lights Total Heating Energy", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Zone Electric Equipment Total Heating Energy", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Zone Gas Equipment Total Heating Energy", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Zone Hot Water Equipment Total Heating Energy", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Zone Steam Equipment Total Heating Energy", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Zone Other Equipment Total Heating Energy", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Zone Infiltration Sensible Heat Gain Energy", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Zone Infiltration Sensible Heat Loss Energy", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(6).show) {
            curReport = AddMonthlyReport(state, "SpaceGainComponentsAtCoolingPeakMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Zone Air System Sensible Cooling Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Zone People Total Heating Energy", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Zone Lights Total Heating Energy", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Zone Electric Equipment Total Heating Energy", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Zone Gas Equipment Total Heating Energy", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Zone Hot Water Equipment Total Heating Energy", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Zone Steam Equipment Total Heating Energy", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Zone Other Equipment Total Heating Energy", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Zone Infiltration Sensible Heat Gain Energy", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Zone Infiltration Sensible Heat Loss Energy", "", iAggType::ValueWhenMaxMin);
        }
        if (ort->namedMonthly(7).show) {
            curReport = AddMonthlyReport(state, "EnergyConsumptionElectricityNaturalGasMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Electricity:Facility", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Electricity:Facility", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "NaturalGas:Facility", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "NaturalGas:Facility", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(8).show) {
            curReport = AddMonthlyReport(state, "EnergyConsumptionElectricityGeneratedPropaneMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "ElectricityProduced:Facility", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "ElectricityProduced:Facility", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Propane:Facility", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Propane:Facility", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(9).show) {
            curReport = AddMonthlyReport(state, "EnergyConsumptionDieselFuelOilMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Diesel:Facility", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Diesel:Facility", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "FuelOilNo1:Facility", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "FuelOilNo1:Facility", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "FuelOilNo2:Facility", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "FuelOilNo2:Facility", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(10).show) {
            curReport = AddMonthlyReport(state, "EnergyConsumptionDistrictHeatingCoolingMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "DistrictCooling:Facility", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "DistrictCooling:Facility", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "DistrictHeating:Facility", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "DistrictHeating:Facility", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(11).show) {
            curReport = AddMonthlyReport(state, "EnergyConsumptionCoalGasolineMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Coal:Facility", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Coal:Facility", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Gasoline:Facility", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Gasoline:Facility", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(12).show) {
            curReport = AddMonthlyReport(state, "EnergyConsumptionOtherFuelsMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "OtherFuel1:Facility", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "OtherFuel1:Facility", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "OtherFuel2:Facility", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "OtherFuel2:Facility", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(13).show) {
            curReport = AddMonthlyReport(state, "EndUseEnergyConsumptionElectricityMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "InteriorLights:Electricity", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorLights:Electricity", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "InteriorEquipment:Electricity", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:Electricity", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Fans:Electricity", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Pumps:Electricity", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Heating:Electricity", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:Electricity", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "HeatRejection:Electricity", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Humidifier:Electricity", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "HeatRecovery:Electricity", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:Electricity", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:Electricity", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(14).show) {
            curReport = AddMonthlyReport(state, "EndUseEnergyConsumptionNaturalGasMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "InteriorEquipment:NaturalGas", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:NaturalGas", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Heating:NaturalGas", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:NaturalGas", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:NaturalGas", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:NaturalGas", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Humidifier:NaturalGas", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(15).show) {
            curReport = AddMonthlyReport(state, "EndUseEnergyConsumptionDieselMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:Diesel", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:Diesel", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Heating:Diesel", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:Diesel", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:Diesel", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(16).show) {
            curReport = AddMonthlyReport(state, "EndUseEnergyConsumptionFuelOilMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:FuelOilNo1", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:FuelOilNo1", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Heating:FuelOilNo1", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:FuelOilNo1", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:FuelOilNo1", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:FuelOilNo2", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:FuelOilNo2", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Heating:FuelOilNo2", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:FuelOilNo2", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:FuelOilNo2", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(17).show) {
            curReport = AddMonthlyReport(state, "EndUseEnergyConsumptionCoalMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:Coal", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Heating:Coal", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:Coal", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(18).show) {
            curReport = AddMonthlyReport(state, "EndUseEnergyConsumptionPropaneMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:Propane", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:Propane", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Heating:Propane", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:Propane", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:Propane", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Humidifier:Propane", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(19).show) {
            curReport = AddMonthlyReport(state, "EndUseEnergyConsumptionGasolineMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:Gasoline", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:Gasoline", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Heating:Gasoline", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:Gasoline", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:Gasoline", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(20).show) {
            curReport = AddMonthlyReport(state, "EndUseEnergyConsumptionOtherFuelsMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:OtherFuel1", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:OtherFuel1", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Heating:OtherFuel1", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:OtherFuel1", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:OtherFuel1", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:OtherFuel2", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:OtherFuel2", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Heating:OtherFuel2", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:OtherFuel2", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:OtherFuel2", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(21).show) {
            curReport = AddMonthlyReport(state, "PeakEnergyEndUseElectricityPart1Monthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "InteriorLights:Electricity", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorLights:Electricity", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "InteriorEquipment:Electricity", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:Electricity", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Fans:Electricity", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Pumps:Electricity", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Heating:Electricity", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(22).show) {
            curReport = AddMonthlyReport(state, "PeakEnergyEndUseElectricityPart2Monthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:Electricity", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "HeatRejection:Electricity", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Humidifier:Electricity", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "HeatRecovery:Electricity", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:Electricity", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:Electricity", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(23).show) {
            curReport = AddMonthlyReport(state, "ElectricComponentsOfPeakDemandMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Electricity:Facility", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "InteriorLights:Electricity", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "InteriorEquipment:Electricity", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorLights:Electricity", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:Electricity", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Fans:Electricity", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Pumps:Electricity", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Heating:Electricity", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:Electricity", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "HeatRejection:Electricity", "", iAggType::ValueWhenMaxMin);
        }
        if (ort->namedMonthly(24).show) {
            curReport = AddMonthlyReport(state, "PeakEnergyEndUseNaturalGasMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "InteriorEquipment:NaturalGas", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:NaturalGas", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Heating:NaturalGas", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:NaturalGas", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:NaturalGas", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:NaturalGas", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(25).show) {
            curReport = AddMonthlyReport(state, "PeakEnergyEndUseDieselMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:Diesel", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:Diesel", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Heating:Diesel", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:Diesel", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:Diesel", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(26).show) {
            curReport = AddMonthlyReport(state, "PeakEnergyEndUseFuelOilMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:FuelOilNo1", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:FuelOilNo1", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Heating:FuelOilNo1", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:FuelOilNo1", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:FuelOilNo1", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:FuelOilNo2", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:FuelOilNo2", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Heating:FuelOilNo2", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:FuelOilNo2", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:FuelOilNo2", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(27).show) {
            curReport = AddMonthlyReport(state, "PeakEnergyEndUseCoalMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:Coal", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Heating:Coal", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:Coal", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(28).show) {
            curReport = AddMonthlyReport(state, "PeakEnergyEndUsePropaneMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:Propane", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:Propane", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Heating:Propane", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:Propane", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:Propane", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(29).show) {
            curReport = AddMonthlyReport(state, "PeakEnergyEndUseGasolineMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:Gasoline", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:Gasoline", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Heating:Gasoline", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:Gasoline", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:Gasoline", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(30).show) {
            curReport = AddMonthlyReport(state, "PeakEnergyEndUseOtherFuelsMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:OtherFuel1", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:OtherFuel1", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Heating:OtherFuel1", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:OtherFuel1", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:OtherFuel1", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "ExteriorEquipment:OtherFuel2", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling:OtherFuel2", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Heating:OtherFuel2", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "WaterSystems:OtherFuel2", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cogeneration:OtherFuel2", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(31).show) {
            curReport = AddMonthlyReport(state, "SetpointsNotMetWithTemperaturesMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Zone Heating Setpoint Not Met Time", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mean Air Temperature", "", iAggType::SumOrAverageHoursShown);
            AddMonthlyFieldSetInput(state, curReport, "Zone Heating Setpoint Not Met While Occupied Time", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mean Air Temperature", "", iAggType::SumOrAverageHoursShown);
            AddMonthlyFieldSetInput(state, curReport, "Zone Cooling Setpoint Not Met Time", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mean Air Temperature", "", iAggType::SumOrAverageHoursShown);
            AddMonthlyFieldSetInput(state, curReport, "Zone Cooling Setpoint Not Met While Occupied Time", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mean Air Temperature", "", iAggType::SumOrAverageHoursShown);
        }
        if (ort->namedMonthly(32).show) {
            curReport = AddMonthlyReport(state, "ComfortReportSimple55Monthly", 2);
            AddMonthlyFieldSetInput(state,
                curReport, "Zone Thermal Comfort ASHRAE 55 Simple Model Summer Clothes Not Comfortable Time", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mean Air Temperature", "", iAggType::SumOrAverageHoursShown);
            AddMonthlyFieldSetInput(state,
                curReport, "Zone Thermal Comfort ASHRAE 55 Simple Model Winter Clothes Not Comfortable Time", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mean Air Temperature", "", iAggType::SumOrAverageHoursShown);
            AddMonthlyFieldSetInput(state,
                curReport, "Zone Thermal Comfort ASHRAE 55 Simple Model Summer or Winter Clothes Not Comfortable Time", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mean Air Temperature", "", iAggType::SumOrAverageHoursShown);
        }
        if (ort->namedMonthly(33).show) {
            curReport = AddMonthlyReport(state, "UnglazedTranspiredSolarCollectorSummaryMonthly", 5);
            AddMonthlyFieldSetInput(state, curReport, "Solar Collector System Efficiency", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Solar Collector System Efficiency", "", iAggType::SumOrAverageHoursShown);
            AddMonthlyFieldSetInput(state, curReport, "Solar Collector Outside Face Suction Velocity", "", iAggType::SumOrAverageHoursShown);
            AddMonthlyFieldSetInput(state, curReport, "Solar Collector Sensible Heating Rate", "", iAggType::SumOrAverageHoursShown);
        }
        if (ort->namedMonthly(34).show) {
            curReport = AddMonthlyReport(state, "OccupantComfortDataSummaryMonthly", 5);
            AddMonthlyFieldSetInput(state, curReport, "People Occupant Count", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "People Air Temperature", "", iAggType::SumOrAverageHoursShown);
            AddMonthlyFieldSetInput(state, curReport, "People Air Relative Humidity", "", iAggType::SumOrAverageHoursShown);
            AddMonthlyFieldSetInput(state, curReport, "Zone Thermal Comfort Fanger Model PMV", "", iAggType::SumOrAverageHoursShown);
            AddMonthlyFieldSetInput(state, curReport, "Zone Thermal Comfort Fanger Model PPD", "", iAggType::SumOrAverageHoursShown);
        }
        if (ort->namedMonthly(35).show) {
            curReport = AddMonthlyReport(state, "ChillerReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Chiller Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Chiller Electricity Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Chiller Electricity Energy", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Chiller Evaporator Cooling Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Chiller Evaporator Cooling Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Chiller Condenser Heat Transfer Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Chiller COP", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Chiller COP", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Chiller Part Load Ratio", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Chiller Part Load Ratio", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(36).show) {
            curReport = AddMonthlyReport(state, "TowerReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Tower Fan Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Tower Fan Electricity Energy", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Tower Fan Electricity Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Tower Heat Transfer Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Tower Inlet Temperature", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Tower Outlet Temperature", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Tower Mass Flow Rate", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(37).show) {
            curReport = AddMonthlyReport(state, "BoilerReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Boiler Heating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Boiler Gas Consumption", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Boiler Heating Energy", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Boiler Heating Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Boiler Gas Consumption Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Boiler Inlet Temperature", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Boiler Outlet Temperature", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Boiler Mass Flow Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Boiler Ancillary Electricity Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Boiler Part Load Ratio", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Boiler Part Load Ratio", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(38).show) {
            curReport = AddMonthlyReport(state, "DXReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Total Cooling Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Total Cooling Energy", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Sensible Cooling Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Latent Cooling Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Crankcase Heater Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Runtime Fraction", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Runtime Fraction", "", iAggType::Minimum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Total Cooling Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Sensible Cooling Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Latent Cooling Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Electricity Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Crankcase Heater Electricity Rate", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(39).show) {
            curReport = AddMonthlyReport(state, "WindowReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Surface Window Transmitted Solar Radiation Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Surface Window Transmitted Beam Solar Radiation Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Surface Window Transmitted Diffuse Solar Radiation Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Surface Window Heat Gain Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Surface Window Heat Loss Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Surface Window Inside Face Glazing Condensation Status", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Surface Shading Device Is On Time Fraction", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Surface Storm Window On Off Status", "", iAggType::HoursNonZero);
        }
        if (ort->namedMonthly(40).show) {
            curReport = AddMonthlyReport(state, "WindowEnergyReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Surface Window Transmitted Solar Radiation Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Surface Window Transmitted Beam Solar Radiation Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Surface Window Transmitted Diffuse Solar Radiation Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Surface Window Heat Gain Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Surface Window Heat Loss Energy", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(41).show) {
            curReport = AddMonthlyReport(state, "WindowZoneSummaryMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Zone Windows Total Heat Gain Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Windows Total Heat Loss Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Windows Total Transmitted Solar Radiation Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Exterior Windows Total Transmitted Beam Solar Radiation Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Exterior Windows Total Transmitted Diffuse Solar Radiation Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Interior Windows Total Transmitted Diffuse Solar Radiation Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Interior Windows Total Transmitted Beam Solar Radiation Rate", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(42).show) {
            curReport = AddMonthlyReport(state, "WindowEnergyZoneSummaryMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Zone Windows Total Heat Gain Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Windows Total Heat Loss Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Windows Total Transmitted Solar Radiation Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Exterior Windows Total Transmitted Beam Solar Radiation Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Exterior Windows Total Transmitted Diffuse Solar Radiation Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Interior Windows Total Transmitted Diffuse Solar Radiation Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Interior Windows Total Transmitted Beam Solar Radiation Energy", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(43).show) {
            curReport = AddMonthlyReport(state, "AverageOutdoorConditionsMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Drybulb Temperature", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Wetbulb Temperature", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Dewpoint Temperature", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Site Wind Speed", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Site Sky Temperature", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Site Diffuse Solar Radiation Rate per Area", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Site Direct Solar Radiation Rate per Area", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Site Rain Status", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(44).show) {
            curReport = AddMonthlyReport(state, "OutdoorConditionsMaximumDryBulbMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Drybulb Temperature", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Wetbulb Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Dewpoint Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Wind Speed", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Sky Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Diffuse Solar Radiation Rate per Area", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Direct Solar Radiation Rate per Area", "", iAggType::ValueWhenMaxMin);
        }
        if (ort->namedMonthly(45).show) {
            curReport = AddMonthlyReport(state, "OutdoorConditionsMinimumDryBulbMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Drybulb Temperature", "", iAggType::Minimum);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Wetbulb Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Dewpoint Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Wind Speed", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Sky Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Diffuse Solar Radiation Rate per Area", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Direct Solar Radiation Rate per Area", "", iAggType::ValueWhenMaxMin);
        }
        if (ort->namedMonthly(46).show) {
            curReport = AddMonthlyReport(state, "OutdoorConditionsMaximumWetBulbMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Wetbulb Temperature", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Drybulb Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Dewpoint Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Wind Speed", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Sky Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Diffuse Solar Radiation Rate per Area", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Direct Solar Radiation Rate per Area", "", iAggType::ValueWhenMaxMin);
        }
        if (ort->namedMonthly(47).show) {
            curReport = AddMonthlyReport(state, "OutdoorConditionsMaximumDewPointMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Dewpoint Temperature", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Drybulb Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Outdoor Air Wetbulb Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Wind Speed", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Sky Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Diffuse Solar Radiation Rate per Area", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Site Direct Solar Radiation Rate per Area", "", iAggType::ValueWhenMaxMin);
        }
        if (ort->namedMonthly(48).show) {
            curReport = AddMonthlyReport(state, "OutdoorGroundConditionsMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Site Ground Temperature", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Site Surface Ground Temperature", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Site Deep Ground Temperature", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Site Mains Water Temperature", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Site Ground Reflected Solar Radiation Rate per Area", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Site Snow on Ground Status", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(49).show) {
            curReport = AddMonthlyReport(state, "WindowACReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Zone Window Air Conditioner Total Cooling Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Window Air Conditioner Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Window Air Conditioner Total Cooling Energy", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Zone Window Air Conditioner Sensible Cooling Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Window Air Conditioner Latent Cooling Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Window Air Conditioner Total Cooling Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Zone Window Air Conditioner Sensible Cooling Rate", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Zone Window Air Conditioner Latent Cooling Rate", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Zone Window Air Conditioner Electricity Rate", "", iAggType::ValueWhenMaxMin);
        }
        if (ort->namedMonthly(50).show) {
            curReport = AddMonthlyReport(state, "WaterHeaterReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Water Heater Total Demand Heat Transfer Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Water Heater Use Side Heat Transfer Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Water Heater Burner Heating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Water Heater Gas Consumption", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Water Heater Total Demand Heat Transfer Energy", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Water Heater Loss Demand Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Water Heater Heat Loss Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Water Heater Tank Temperature", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Water Heater Heat Recovery Supply Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Water Heater Source Energy", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(51).show) {
            curReport = AddMonthlyReport(state, "GeneratorReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Generator Produced AC Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Generator Diesel Consumption", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Generator Gas Consumption", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Generator Produced AC Electricity Energy", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Generator Total Heat Recovery", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Generator Jacket Heat Recovery Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Generator Lube Heat Recovery", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Generator Exhaust Heat Recovery Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Generator Exhaust Air Temperature", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(52).show) {
            curReport = AddMonthlyReport(state, "DaylightingReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Site Exterior Beam Normal Illuminance", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Daylighting Lighting Power Multiplier", "", iAggType::SumOrAverageHoursShown);
            AddMonthlyFieldSetInput(state, curReport, "Daylighting Lighting Power Multiplier", "", iAggType::MinimumDuringHoursShown);
            AddMonthlyFieldSetInput(state, curReport, "Daylighting Reference Point 1 Illuminance", "", iAggType::SumOrAverageHoursShown);
            AddMonthlyFieldSetInput(state, curReport, "Daylighting Reference Point 1 Glare Index", "", iAggType::SumOrAverageHoursShown);
            AddMonthlyFieldSetInput(state, curReport, "Daylighting Reference Point 1 Glare Index Setpoint Exceeded Time", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Daylighting Reference Point 1 Daylight Illuminance Setpoint Exceeded Time", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Daylighting Reference Point 2 Illuminance", "", iAggType::SumOrAverageHoursShown);
            AddMonthlyFieldSetInput(state, curReport, "Daylighting Reference Point 2 Glare Index", "", iAggType::SumOrAverageHoursShown);
            AddMonthlyFieldSetInput(state, curReport, "Daylighting Reference Point 2 Glare Index Setpoint Exceeded Time", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Daylighting Reference Point 2 Daylight Illuminance Setpoint Exceeded Time", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(53).show) {
            curReport = AddMonthlyReport(state, "CoilReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Heating Coil Heating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Heating Coil Heating Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Total Cooling Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Sensible Cooling Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Total Cooling Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Sensible Cooling Rate", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Cooling Coil Wetted Area Fraction", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(54).show) {
            curReport = AddMonthlyReport(state, "PlantLoopDemandReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Plant Supply Side Cooling Demand Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Plant Supply Side Cooling Demand Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Plant Supply Side Heating Demand Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Plant Supply Side Heating Demand Rate", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(55).show) {
            curReport = AddMonthlyReport(state, "FanReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Fan Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Fan Rise in Air Temperature", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Fan Electricity Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Fan Rise in Air Temperature", "", iAggType::ValueWhenMaxMin);
        }
        if (ort->namedMonthly(56).show) {
            curReport = AddMonthlyReport(state, "PumpReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Pump Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Pump Fluid Heat Gain Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Pump Electricity Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Pump Shaft Power", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Pump Fluid Heat Gain Rate", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Pump Outlet Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Pump Mass Flow Rate", "", iAggType::ValueWhenMaxMin);
        }
        if (ort->namedMonthly(57).show) {
            curReport = AddMonthlyReport(state, "CondLoopDemandReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Plant Supply Side Cooling Demand Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Plant Supply Side Cooling Demand Rate", "", iAggType::Maximum);
            AddMonthlyFieldSetInput(state, curReport, "Plant Supply Side Inlet Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Plant Supply Side Outlet Temperature", "", iAggType::ValueWhenMaxMin);
            AddMonthlyFieldSetInput(state, curReport, "Plant Supply Side Heating Demand Rate", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Plant Supply Side Heating Demand Rate", "", iAggType::Maximum);
        }
        if (ort->namedMonthly(58).show) {
            curReport = AddMonthlyReport(state, "ZoneTemperatureOscillationReportMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Zone Oscillating Temperatures Time", "", iAggType::HoursNonZero);
            AddMonthlyFieldSetInput(state, curReport, "Zone People Occupant Count", "", iAggType::SumOrAverageHoursShown);
        }
        if (ort->namedMonthly(59).show) {
            curReport = AddMonthlyReport(state, "AirLoopSystemEnergyAndWaterUseMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Air System Hot Water Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Steam Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Chilled Water Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Natural Gas Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Water Volume", "", iAggType::SumOrAvg);
        }

        if (ort->namedMonthly(60).show) {
            curReport = AddMonthlyReport(state, "AirLoopSystemComponentLoadsMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Air System Fan Air Heating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Cooling Coil Total Cooling Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Heating Coil Total Heating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Heat Exchanger Total Heating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Heat Exchanger Total Cooling Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Humidifier Total Heating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Evaporative Cooler Total Cooling Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Desiccant Dehumidifier Total Cooling Energy", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(61).show) {
            curReport = AddMonthlyReport(state, "AirLoopSystemComponentEnergyUseMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Air System Fan Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Heating Coil Hot Water Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Cooling Coil Chilled Water Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System DX Heating Coil Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System DX Cooling Coil Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Heating Coil Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Heating Coil Natural Gas Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Heating Coil Steam Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Humidifier Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Humidifier Natural Gas Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Evaporative Cooler Electricity Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Desiccant Dehumidifier Electricity Energy", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(62).show) {
            curReport = AddMonthlyReport(state, "MechanicalVentilationLoadsMonthly", 2);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mechanical Ventilation No Load Heat Removal Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mechanical Ventilation Cooling Load Increase Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mechanical Ventilation Cooling Load Increase Due to Overheating Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mechanical Ventilation Cooling Load Decrease Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mechanical Ventilation No Load Heat Addition Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mechanical Ventilation Heating Load Increase Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mechanical Ventilation Heating Load Increase Due to Overcooling Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mechanical Ventilation Heating Load Decrease Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Zone Mechanical Ventilation Air Changes per Hour", "", iAggType::SumOrAvg);
        }
        if (ort->namedMonthly(63).show) {
            curReport = AddMonthlyReport(state, "HeatEmissionsReportMonthly", 2);
            // Place holder
            AddMonthlyFieldSetInput(state, curReport, "Site Total Surface Heat Emission to Air", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Site Total Zone Exfiltration Heat Loss", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Site Total Zone Exhaust Air Heat Loss", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "Air System Relief Air Total Heat Loss Energy", "", iAggType::SumOrAvg);
            AddMonthlyFieldSetInput(state, curReport, "HVAC System Total Heat Rejection Energy", "", iAggType::SumOrAvg);
        }
    }

    void GetInputFuelAndPollutionFactors(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   January 2004
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Read the Fuel Factor inputs by the user to
        //   get the source energy conversion factors
        //   Also reads PolutionCalculationFactors to
        //   get information on district cooling and heating

        // METHODOLOGY EMPLOYED:
        //   Uses get input structure similar to other objects

        // REFERENCES:
        // na

        // Using/Aliasing
        using PollutionModule::GetEnvironmentalImpactFactorInfo;
        using PollutionModule::GetFuelFactorInfo;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        Real64 curSourceFactor;
        bool fuelFactorUsed;
        bool fFScheduleUsed;
        int ffScheduleIndex;
        auto &ort(state.dataOutRptTab);

        // set the default factors for source energy - they will be overwritten if the user sets any values
        ort->sourceFactorElectric = 3.167;
        ort->sourceFactorNaturalGas = 1.084;
        ort->sourceFactorSteam = 1.20;
        ort->sourceFactorGasoline = 1.05;
        ort->sourceFactorDiesel = 1.05;
        ort->sourceFactorCoal = 1.05;
        ort->sourceFactorFuelOil1 = 1.05;
        ort->sourceFactorFuelOil2 = 1.05;
        ort->sourceFactorPropane = 1.05;
        ort->sourceFactorOtherFuel1 = 1.0;
        ort->sourceFactorOtherFuel2 = 1.0;
        // the following should be kept consistent with the assumptions in the pollution calculation routines
        ort->efficiencyDistrictCooling = 3.0;
        ort->efficiencyDistrictHeating = 0.3;

        //  TotalSourceEnergyUse = (gatherTotalsSource(1) & !total source from electricity
        //                  +  gatherTotalsSource(2)   & !natural gas
        //                  + gatherTotalsSource(3)    & !gasoline
        //                  + gatherTotalsSource(4)    & !diesel
        //                  + gatherTotalsSource(5)    & !coal
        //                  + gatherTotalsSource(6)    & !Fuel Oil No1
        //                  + gatherTotalsSource(7)    & !Fuel Oil No2
        //                  + gatherTotalsSource(8)    &  !propane
        //                  + gatherTotalsBEPS(3)*sourceFactorElectric/efficiencyDistrictCooling  & !district cooling
        //                  + gatherTotalsBEPS(4)*sourceFactorNaturalGas/efficiencyDistrictHeating  & !district heating
        //                  + gatherTotalsBEPS(5)*sourceFactorSteam  & !steam
        //                                          ) / largeConversionFactor

        GetFuelFactorInfo(state, "NaturalGas", fuelFactorUsed, curSourceFactor, fFScheduleUsed, ffScheduleIndex);
        if (fuelFactorUsed) {
            ort->sourceFactorNaturalGas = curSourceFactor;
            ort->fuelfactorsused(2) = true;
            ort->ffUsed(2) = true;
        }
        ort->SourceFactors(2) = curSourceFactor;
        if (fFScheduleUsed) {
            ort->fuelFactorSchedulesUsed = true;
            ort->ffSchedUsed(2) = true;
            ort->ffSchedIndex(2) = ffScheduleIndex;
        }

        GetFuelFactorInfo(state, "FuelOilNo2", fuelFactorUsed, curSourceFactor, fFScheduleUsed, ffScheduleIndex);
        if (fuelFactorUsed) {
            ort->sourceFactorFuelOil2 = curSourceFactor;
            ort->fuelfactorsused(7) = true;
            ort->ffUsed(11) = true;
        }
        ort->SourceFactors(11) = curSourceFactor;
        if (fFScheduleUsed) {
            ort->fuelFactorSchedulesUsed = true;
            ort->ffSchedUsed(11) = true;
            ort->ffSchedIndex(11) = ffScheduleIndex;
        }

        GetFuelFactorInfo(state, "FuelOilNo1", fuelFactorUsed, curSourceFactor, fFScheduleUsed, ffScheduleIndex);
        if (fuelFactorUsed) {
            ort->sourceFactorFuelOil1 = curSourceFactor;
            ort->fuelfactorsused(6) = true;
            ort->ffUsed(10) = true;
        }
        ort->SourceFactors(10) = curSourceFactor;
        if (fFScheduleUsed) {
            ort->fuelFactorSchedulesUsed = true;
            ort->ffSchedUsed(10) = true;
            ort->ffSchedIndex(10) = ffScheduleIndex;
        }

        GetFuelFactorInfo(state, "Coal", fuelFactorUsed, curSourceFactor, fFScheduleUsed, ffScheduleIndex);
        if (fuelFactorUsed) {
            ort->sourceFactorCoal = curSourceFactor;
            ort->fuelfactorsused(5) = true;
            ort->ffUsed(9) = true;
        }
        ort->SourceFactors(9) = curSourceFactor;
        if (fFScheduleUsed) {
            ort->fuelFactorSchedulesUsed = true;
            ort->ffSchedUsed(9) = true;
            ort->ffSchedIndex(9) = ffScheduleIndex;
        }

        GetFuelFactorInfo(state, "Electricity", fuelFactorUsed, curSourceFactor, fFScheduleUsed, ffScheduleIndex);
        if (fuelFactorUsed) {
            ort->sourceFactorElectric = curSourceFactor;
            ort->fuelfactorsused(1) = true;
            ort->ffUsed(1) = true;
        }
        ort->SourceFactors(1) = curSourceFactor;
        if (fFScheduleUsed) {
            ort->fuelFactorSchedulesUsed = true;
            ort->ffSchedUsed(1) = true;
            ort->ffSchedIndex(1) = ffScheduleIndex;
        }

        GetFuelFactorInfo(state, "Gasoline", fuelFactorUsed, curSourceFactor, fFScheduleUsed, ffScheduleIndex);
        if (fuelFactorUsed) {
            ort->sourceFactorGasoline = curSourceFactor;
            ort->fuelfactorsused(3) = true;
            ort->ffUsed(6) = true;
        }
        ort->SourceFactors(6) = curSourceFactor;
        if (fFScheduleUsed) {
            ort->fuelFactorSchedulesUsed = true;
            ort->ffSchedUsed(6) = true;
            ort->ffSchedIndex(6) = ffScheduleIndex;
        }

        GetFuelFactorInfo(state, "Propane", fuelFactorUsed, curSourceFactor, fFScheduleUsed, ffScheduleIndex);
        if (fuelFactorUsed) {
            ort->sourceFactorPropane = curSourceFactor;
            ort->fuelfactorsused(8) = true;
            ort->ffUsed(12) = true;
        }
        ort->SourceFactors(12) = curSourceFactor;
        if (fFScheduleUsed) {
            ort->fuelFactorSchedulesUsed = true;
            ort->ffSchedUsed(12) = true;
            ort->ffSchedIndex(12) = ffScheduleIndex;
        }

        GetFuelFactorInfo(state, "Diesel", fuelFactorUsed, curSourceFactor, fFScheduleUsed, ffScheduleIndex);
        if (fuelFactorUsed) {
            ort->sourceFactorDiesel = curSourceFactor;
            ort->fuelfactorsused(4) = true;
            ort->ffUsed(8) = true;
        }
        ort->SourceFactors(8) = curSourceFactor;
        if (fFScheduleUsed) {
            ort->fuelFactorSchedulesUsed = true;
            ort->ffSchedUsed(8) = true;
            ort->ffSchedIndex(8) = ffScheduleIndex;
        }

        GetFuelFactorInfo(state, "DistrictCooling", fuelFactorUsed, curSourceFactor, fFScheduleUsed, ffScheduleIndex);
        if (fuelFactorUsed) {
            ort->ffUsed(3) = true;
        }
        ort->SourceFactors(3) = curSourceFactor;
        if (fFScheduleUsed) {
            ort->ffSchedUsed(3) = true;
            ort->ffSchedIndex(3) = ffScheduleIndex;
        }

        GetFuelFactorInfo(state, "DistrictHeating", fuelFactorUsed, curSourceFactor, fFScheduleUsed, ffScheduleIndex);
        if (fuelFactorUsed) {
            ort->ffUsed(4) = true;
        }
        ort->SourceFactors(4) = curSourceFactor;
        if (fFScheduleUsed) {
            ort->ffSchedUsed(4) = true;
            ort->ffSchedIndex(4) = ffScheduleIndex;
        }

        GetFuelFactorInfo(state, "Steam", fuelFactorUsed, curSourceFactor, fFScheduleUsed, ffScheduleIndex);
        if (fuelFactorUsed) {
            ort->ffUsed(5) = true;
        }
        ort->SourceFactors(5) = curSourceFactor;
        if (fFScheduleUsed) {
            ort->ffSchedUsed(5) = true;
            ort->ffSchedIndex(5) = ffScheduleIndex;
        }

        GetFuelFactorInfo(state, "OtherFuel1", fuelFactorUsed, curSourceFactor, fFScheduleUsed, ffScheduleIndex);
        if (fuelFactorUsed) {
            ort->sourceFactorOtherFuel1 = curSourceFactor;
            ort->fuelfactorsused(11) = true; // should be source number
            ort->ffUsed(13) = true;
        }
        ort->SourceFactors(13) = curSourceFactor;
        if (fFScheduleUsed) {
            ort->fuelFactorSchedulesUsed = true;
            ort->ffSchedUsed(13) = true;
            ort->ffSchedIndex(13) = ffScheduleIndex;
        }

        GetFuelFactorInfo(state, "OtherFuel2", fuelFactorUsed, curSourceFactor, fFScheduleUsed, ffScheduleIndex);
        if (fuelFactorUsed) {
            ort->sourceFactorOtherFuel2 = curSourceFactor;
            ort->fuelfactorsused(12) = true; // should be source number
            ort->ffUsed(14) = true;
        }
        ort->SourceFactors(14) = curSourceFactor;
        if (fFScheduleUsed) {
            ort->fuelFactorSchedulesUsed = true;
            ort->ffSchedUsed(14) = true;
            ort->ffSchedIndex(14) = ffScheduleIndex;
        }

        GetEnvironmentalImpactFactorInfo(state, ort->efficiencyDistrictHeating, ort->efficiencyDistrictCooling, ort->sourceFactorSteam);
    }

    //======================================================================================================================
    //======================================================================================================================

    //    OTHER INITIALIZATION ROUTINES

    //======================================================================================================================
    //======================================================================================================================

    void OpenOutputTabularFile(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   July 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Create a file that holds the output from the tabular reports
        //   the output is in a CSV file if it is comma delimited otherwise
        //   it is in a TXT file.

        // METHODOLOGY EMPLOYED:
        //   Uses get input structure similar to other objects

        // REFERENCES:
        // na

        // Using/Aliasing
        using DataHeatBalance::BuildingName;
        using DataStringGlobals::VerString;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int iStyle;
        std::string curDel;
        auto &ort(state.dataOutRptTab);

        // get a new file unit number
        // create a file to hold the results
        // Use a CSV file if comma separated but otherwise use TXT file
        // extension.
        if (ort->WriteTabularFiles && state.files.outputControl.tabular) {
            for (iStyle = 1; iStyle <= ort->numStyles; ++iStyle) {
                curDel = ort->del(iStyle);
                if (ort->TableStyle(iStyle) == iTableStyle::Comma) {
                    DisplayString(state, "Writing tabular output file results using comma format.");
                    std::ofstream & tbl_stream = open_tbl_stream(state, iStyle, DataStringGlobals::outputTblCsvFileName, state.files.outputControl.tabular);
                    tbl_stream << "Program Version:" << curDel << VerString << '\n';
                    tbl_stream << "Tabular Output Report in Format: " << curDel << "Comma\n";
                    tbl_stream << '\n';
                    tbl_stream << "Building:" << curDel << BuildingName << '\n';
                    if (state.dataEnvrn->EnvironmentName == state.dataEnvrn->WeatherFileLocationTitle) {
                        tbl_stream << "Environment:" << curDel << state.dataEnvrn->EnvironmentName << '\n';
                    } else {
                        tbl_stream << "Environment:" << curDel << state.dataEnvrn->EnvironmentName << " ** " << state.dataEnvrn->WeatherFileLocationTitle << '\n';
                    }
                    tbl_stream << '\n';
                } else if (ort->TableStyle(iStyle) == iTableStyle::Tab) {
                    DisplayString(state, "Writing tabular output file results using tab format.");
                    std::ofstream & tbl_stream = open_tbl_stream(state, iStyle, DataStringGlobals::outputTblTabFileName, state.files.outputControl.tabular);
                    tbl_stream << "Program Version" << curDel << VerString << '\n';
                    tbl_stream << "Tabular Output Report in Format: " << curDel << "Tab\n";
                    tbl_stream << '\n';
                    tbl_stream << "Building:" << curDel << BuildingName << '\n';
                    if (state.dataEnvrn->EnvironmentName == state.dataEnvrn->WeatherFileLocationTitle) {
                        tbl_stream << "Environment:" << curDel << state.dataEnvrn->EnvironmentName << '\n';
                    } else {
                        tbl_stream << "Environment:" << curDel << state.dataEnvrn->EnvironmentName << " ** " << state.dataEnvrn->WeatherFileLocationTitle << '\n';
                    }
                    tbl_stream << '\n';
                } else if (ort->TableStyle(iStyle) == iTableStyle::HTML) {
                    DisplayString(state, "Writing tabular output file results using HTML format.");
                    std::ofstream & tbl_stream = open_tbl_stream(state, iStyle, DataStringGlobals::outputTblHtmFileName, state.files.outputControl.tabular);
                    tbl_stream << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\"http://www.w3.org/TR/html4/loose.dtd\">\n";
                    tbl_stream << "<html>\n";
                    tbl_stream << "<head>\n";
                    if (state.dataEnvrn->EnvironmentName == state.dataEnvrn->WeatherFileLocationTitle) {
                        tbl_stream << "<title> " << BuildingName << ' ' << state.dataEnvrn->EnvironmentName << '\n';
                    } else {
                        tbl_stream << "<title> " << BuildingName << ' ' << state.dataEnvrn->EnvironmentName << " ** " << state.dataEnvrn->WeatherFileLocationTitle << '\n';
                    }
                    tbl_stream << "  " << std::setw(4) << ort->td(1) << '-' << std::setfill('0') << std::setw(2) << ort->td(2) << '-' << std::setw(2) << ort->td(3)
                               << '\n';
                    tbl_stream << "  " << std::setw(2) << ort->td(5) << ':' << std::setw(2) << ort->td(6) << ':' << std::setw(2) << ort->td(7) << std::setfill(' ')
                               << '\n';
                    tbl_stream << " - EnergyPlus</title>\n";
                    tbl_stream << "</head>\n";
                    tbl_stream << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n";
                    tbl_stream << "<body>\n";
                    tbl_stream << "<p><a href=\"#toc\" style=\"float: right\">Table of Contents</a></p>\n";
                    tbl_stream << "<a name=top></a>\n";
                    tbl_stream << "<p>Program Version:<b>" << VerString << "</b></p>\n";
                    tbl_stream << "<p>Tabular Output Report in Format: <b>HTML</b></p>\n";
                    tbl_stream << "<p>Building: <b>" << BuildingName << "</b></p>\n";
                    if (state.dataEnvrn->EnvironmentName == state.dataEnvrn->WeatherFileLocationTitle) {
                        tbl_stream << "<p>Environment: <b>" << state.dataEnvrn->EnvironmentName << "</b></p>\n";
                    } else {
                        tbl_stream << "<p>Environment: <b>" << state.dataEnvrn->EnvironmentName << " ** " << state.dataEnvrn->WeatherFileLocationTitle << "</b></p>\n";
                    }
                    tbl_stream << "<p>Simulation Timestamp: <b>" << std::setw(4) << ort->td(1) << '-' << std::setfill('0') << std::setw(2) << ort->td(2) << '-'
                               << std::setw(2) << ort->td(3) << '\n';
                    tbl_stream << "  " << std::setw(2) << ort->td(5) << ':' << std::setw(2) << ort->td(6) << ':' << std::setw(2) << ort->td(7) << std::setfill(' ')
                               << "</b></p>\n";
                } else if (ort->TableStyle(iStyle) == iTableStyle::XML) {
                    DisplayString(state, "Writing tabular output file results using XML format.");
                    std::ofstream & tbl_stream = open_tbl_stream(state, iStyle, DataStringGlobals::outputTblXmlFileName, state.files.outputControl.tabular);
                    tbl_stream << "<?xml version=\"1.0\"?>\n";
                    tbl_stream << "<EnergyPlusTabularReports>\n";
                    tbl_stream << "  <BuildingName>" << BuildingName << "</BuildingName>\n";
                    tbl_stream << "  <EnvironmentName>" << state.dataEnvrn->EnvironmentName << "</EnvironmentName>\n";
                    tbl_stream << "  <WeatherFileLocationTitle>" << state.dataEnvrn->WeatherFileLocationTitle << "</WeatherFileLocationTitle>\n";
                    tbl_stream << "  <ProgramVersion>" << VerString << "</ProgramVersion>\n";
                    tbl_stream << "  <SimulationTimestamp>\n";
                    tbl_stream << "    <Date>\n";
                    tbl_stream << "      " << std::setw(4) << ort->td(1) << '-' << std::setfill('0') << std::setw(2) << ort->td(2) << '-' << std::setw(2)
                               << ort->td(3) << '\n';
                    tbl_stream << "    </Date>\n";
                    tbl_stream << "    <Time>\n";
                    tbl_stream << "      " << std::setw(2) << ort->td(5) << ':' << std::setw(2) << ort->td(6) << ':' << std::setw(2) << ort->td(7)
                               << std::setfill(' ') << '\n';
                    tbl_stream << "    </Time>\n";
                    tbl_stream << "  </SimulationTimestamp>\n";
                    tbl_stream << '\n';
                } else {
                    DisplayString(state, "Writing tabular output file results using text format.");
                    std::ofstream & tbl_stream = open_tbl_stream(state, iStyle, DataStringGlobals::outputTblTxtFileName, state.files.outputControl.tabular);
                    tbl_stream << "Program Version: " << VerString << '\n';
                    tbl_stream << "Tabular Output Report in Format: " << curDel << "Fixed\n";
                    tbl_stream << '\n';
                    tbl_stream << "Building:        " << BuildingName << '\n';
                    if (state.dataEnvrn->EnvironmentName == state.dataEnvrn->WeatherFileLocationTitle) {
                        tbl_stream << "Environment:     " << state.dataEnvrn->EnvironmentName << '\n';
                    } else {
                        tbl_stream << "Environment:     " << state.dataEnvrn->EnvironmentName << " ** " << state.dataEnvrn->WeatherFileLocationTitle << '\n';
                    }
                    tbl_stream << '\n';
                }
            }
        }
    }

    void CloseOutputTabularFile(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   July 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Create a file that holds the output from the tabular reports
        //   the output is in a CSV file if it is comma delimited otherwise
        //   it is in a TXT file.

        // METHODOLOGY EMPLOYED:
        //   Uses get input structure similar to other objects

        // REFERENCES:
        // na

        // USE STATEMENTS:
        // na

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int iStyle;
        auto &ort(state.dataOutRptTab);

        if (ort->WriteTabularFiles) {
            for (iStyle = 1; iStyle <= ort->numStyles; ++iStyle) {
                std::ofstream &tbl_stream(*ort->TabularOutputFile(iStyle));
                if (ort->TableStyle(iStyle) == iTableStyle::HTML) { // if HTML file put ending info
                    tbl_stream << "</body>\n";
                    tbl_stream << "</html>\n";
                } else if (ort->TableStyle(iStyle) == iTableStyle::XML) {
                    if (!ort->prevReportName.empty()) {
                        tbl_stream << "</" << ort->prevReportName << ">\n"; // close the last element if it was used.
                    }
                    tbl_stream << "</EnergyPlusTabularReports>\n";
                }
                tbl_stream.close();
            }
        }
    }

    void WriteTableOfContents(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   June 2005
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Creates hyperlinks for table of contents

        // METHODOLOGY EMPLOYED:
        //   Go through the reports and create links

        // REFERENCES:
        // na

        // Using/Aliasing
        using EconomicLifeCycleCost::LCCparamPresent;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        static std::string const Entire_Facility("Entire Facility");
        static std::string const Annual_Building_Utility_Performance_Summary("Annual Building Utility Performance Summary");
        static std::string const Input_Verification_and_Results_Summary("Input Verification and Results Summary");
        static std::string const Demand_End_Use_Components_Summary("Demand End Use Components Summary");
        static std::string const Source_Energy_End_Use_Components_Summary("Source Energy End Use Components Summary");
        static std::string const Component_Cost_Economics_Summary("Component Cost Economics Summary");
        static std::string const Component_Sizing_Summary("Component Sizing Summary");
        static std::string const Surface_Shadowing_Summary("Surface Shadowing Summary");
        static std::string const Adaptive_Comfort_Summary("Adaptive Comfort Summary");
        static std::string const Initialization_Summary("Initialization Summary");
        static std::string const Annual_Heat_Emissions_Summary("Annual Heat Emissions Summary");
        static std::string const Annual_Thermal_Resilience_Summary("Annual Thermal Resilience Summary");
        static std::string const Annual_CO2_Resilience_Summary("Annual CO2 Resilience Summary");
        static std::string const Annual_Visual_Resilience_Summary("Annual Visual Resilience Summary");

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int iInput;
        int jTable;
        int curTable;
        int iEntry;
        int jEntry;
        int kReport;
        std::string curSection;
        int iStyle;
        std::string origName;
        std::string curName;
        int indexUnitConv;
        auto &ort(state.dataOutRptTab);

        // normally do not add to the table of contents here but the order of calls is different for the life-cycle costs
        if (ort->displayLifeCycleCostReport && LCCparamPresent) {
            AddTOCEntry(state, "Life-Cycle Cost Report", "Entire Facility");
        }

        for (iStyle = 1; iStyle <= ort->numStyles; ++iStyle) {
            if (ort->TableStyle(iStyle) == iTableStyle::HTML) {
                std::ostream &tbl_stream(*ort->TabularOutputFile(iStyle));
                tbl_stream << "<hr>\n";
                tbl_stream << "<a name=toc></a>\n";
                tbl_stream << "<p><b>Table of Contents</b></p>\n";
                tbl_stream << "<a href=\"#top\">Top</a>\n";
                if (ort->displayTabularBEPS) {
                    tbl_stream << "<br><a href=\"#" << MakeAnchorName(Annual_Building_Utility_Performance_Summary, Entire_Facility)
                               << "\">Annual Building Utility Performance Summary</a>\n";
                }
                if (ort->displayTabularVeriSum) {
                    tbl_stream << "<br><a href=\"#" << MakeAnchorName(Input_Verification_and_Results_Summary, Entire_Facility)
                               << "\">Input Verification and Results Summary</a>\n";
                }
                if (ort->displayDemandEndUse) {
                    tbl_stream << "<br><a href=\"#" << MakeAnchorName(Demand_End_Use_Components_Summary, Entire_Facility)
                               << "\">Demand End Use Components Summary</a>\n";
                }
                if (ort->displaySourceEnergyEndUseSummary) {
                    tbl_stream << "<br><a href=\"#" << MakeAnchorName(Source_Energy_End_Use_Components_Summary, Entire_Facility)
                               << "\">Source Energy End Use Components Summary</a>\n";
                }
                if (state.dataCostEstimateManager->DoCostEstimate) {
                    tbl_stream << "<br><a href=\"#" << MakeAnchorName(Component_Cost_Economics_Summary, Entire_Facility)
                               << "\">Component Cost Economics Summary</a>\n";
                }
                if (ort->displayComponentSizing) {
                    tbl_stream << "<br><a href=\"#" << MakeAnchorName(Component_Sizing_Summary, Entire_Facility)
                               << "\">Component Sizing Summary</a>\n";
                }
                if (ort->displaySurfaceShadowing) {
                    tbl_stream << "<br><a href=\"#" << MakeAnchorName(Surface_Shadowing_Summary, Entire_Facility)
                               << "\">Surface Shadowing Summary</a>\n";
                }
                if (ort->displayAdaptiveComfort) {
                    tbl_stream << "<br><a href=\"#" << MakeAnchorName(Adaptive_Comfort_Summary, Entire_Facility)
                               << "\">Adaptive Comfort Summary</a>\n";
                }
                if (ort->displayEioSummary) {
                    tbl_stream << "<br><a href=\"#" << MakeAnchorName(Initialization_Summary, Entire_Facility) << "\">Initialization Summary</a>\n";
                }
                if (ort->displayHeatEmissionsSummary) {
                    tbl_stream << "<br><a href=\"#" << MakeAnchorName(Annual_Heat_Emissions_Summary, Entire_Facility)
                               << "\">Annual Heat Emissions Summary</a>\n";
                }
//                if (displayThermalResilienceSummary) {
//                    tbl_stream << "<br><a href=\"#" << MakeAnchorName(Annual_Thermal_Resilience_Summary, Entire_Facility)
//                               << "\">Annual Thermal Resilience Summary</a>\n";
//                }
//                if (displayCO2ResilienceSummary) {
//                    tbl_stream << "<br><a href=\"#" << MakeAnchorName(Annual_CO2_Resilience_Summary, Entire_Facility)
//                               << "\">Annual CO2 Resilience Summary</a>\n";
//                }
//                if (displayVisualResilienceSummary) {
//                    tbl_stream << "<br><a href=\"#" << MakeAnchorName(Annual_Visual_Resilience_Summary, Entire_Facility)
//                               << "\">Annual Visual Resilience Summary</a>\n";
//                }
                for (kReport = 1; kReport <= state.dataOutRptPredefined->numReportName; ++kReport) {
                    if (state.dataOutRptPredefined->reportName(kReport).show) {
                        tbl_stream << "<br><a href=\"#" << MakeAnchorName(state.dataOutRptPredefined->reportName(kReport).namewithspaces, Entire_Facility) << "\">"
                                   << state.dataOutRptPredefined->reportName(kReport).namewithspaces << "</a>\n";
                    }
                }
                if (state.dataGlobal->DoWeathSim) {
                    for (iInput = 1; iInput <= ort->MonthlyInputCount; ++iInput) {
                        if (ort->MonthlyInput(iInput).numTables > 0) {
                            tbl_stream << "<p><b>" << ort->MonthlyInput(iInput).name << "</b></p> |\n";
                            for (jTable = 1; jTable <= ort->MonthlyInput(iInput).numTables; ++jTable) {
                                curTable = jTable + ort->MonthlyInput(iInput).firstTable - 1;
                                tbl_stream << "<a href=\"#" << MakeAnchorName(ort->MonthlyInput(iInput).name, ort->MonthlyTables(curTable).keyValue) << "\">"
                                           << ort->MonthlyTables(curTable).keyValue << "</a>    |   \n";
                            }
                        }
                    }
                    for (iInput = 1; iInput <= ort->OutputTableBinnedCount; ++iInput) {
                        if (ort->OutputTableBinned(iInput).numTables > 0) {
                            if (ort->OutputTableBinned(iInput).scheduleIndex == 0) {
                                tbl_stream << "<p><b>" << ort->OutputTableBinned(iInput).varOrMeter << "</b></p> |\n";
                            } else {
                                tbl_stream << "<p><b>" << ort->OutputTableBinned(iInput).varOrMeter << " [" << ort->OutputTableBinned(iInput).ScheduleName
                                           << "]</b></p> |\n";
                            }
                            for (jTable = 1; jTable <= ort->OutputTableBinned(iInput).numTables; ++jTable) {
                                curTable = ort->OutputTableBinned(iInput).resIndex + (jTable - 1);
                                curName = "";
                                if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                    origName = ort->OutputTableBinned(iInput).varOrMeter + unitEnumToStringBrackets(ort->OutputTableBinned(iInput).units);
                                    LookupSItoIP(state, origName, indexUnitConv, curName);
                                } else {
                                    curName = ort->OutputTableBinned(iInput).varOrMeter + unitEnumToStringBrackets(ort->OutputTableBinned(iInput).units);
                                }
                                if (ort->OutputTableBinned(iInput).scheduleIndex == 0) {
                                    tbl_stream << "<a href=\"#" << MakeAnchorName(curName, ort->BinObjVarID(curTable).namesOfObj) << "\">"
                                               << ort->BinObjVarID(curTable).namesOfObj << "</a>   |  \n";
                                } else {
                                    tbl_stream << "<a href=\"#"
                                               << MakeAnchorName(curName + ort->OutputTableBinned(iInput).ScheduleName, ort->BinObjVarID(curTable).namesOfObj)
                                               << "\">" << ort->BinObjVarID(curTable).namesOfObj << "</a>   |  \n";
                                }
                            }
                        }
                    }
                    OutputReportTabularAnnual::AddAnnualTableOfContents(tbl_stream);
                }
                // add entries specifically added using AddTOCEntry
                for (iEntry = 1; iEntry <= ort->TOCEntriesCount; ++iEntry) {
                    if (!ort->TOCEntries(iEntry).isWritten) {
                        curSection = ort->TOCEntries(iEntry).sectionName;
                        tbl_stream << "<p><b>" << curSection << "</b></p> |\n";
                        for (jEntry = iEntry; jEntry <= ort->TOCEntriesCount; ++jEntry) {
                            if (!ort->TOCEntries(jEntry).isWritten) {
                                if (ort->TOCEntries(jEntry).sectionName == curSection) {
                                    tbl_stream << "<a href=\"#" << MakeAnchorName(ort->TOCEntries(jEntry).sectionName, ort->TOCEntries(jEntry).reportName)
                                               << "\">" << ort->TOCEntries(jEntry).reportName << "</a>   |  \n";
                                    ort->TOCEntries(jEntry).isWritten = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //======================================================================================================================
    //======================================================================================================================

    //    GATHER DATA EACH TIME STEP ROUTINES

    //======================================================================================================================
    //======================================================================================================================

    void GatherBinResultsForTimestep(EnergyPlusData &state, OutputProcessor::TimeStepType t_timeStepType) // What kind of data to update (Zone, HVAC)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   August 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Gathers the data each timesetp and adds the length of the
        //   timestep to the appropriate bin.

        // Using/Aliasing
        using DataHVACGlobals::TimeStepSys;
        using ScheduleManager::GetCurrentScheduleValue;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int iInObj;
        int jTable;
        Real64 curValue;
        // values of OutputTableBinned array for current index
        Real64 curIntervalStart;
        Real64 curIntervalSize;
        int curIntervalCount;
        int curResIndex;
        int curNumTables;
        int curTypeOfVar;
        int curScheduleIndex;
        Real64 elapsedTime;
        bool gatherThisTime;
        Real64 topValue;
        int binNum;
        int repIndex;
        OutputProcessor::TimeStepType curStepType;
        auto &ort(state.dataOutRptTab);

        if (!state.dataGlobal->DoWeathSim) return;
        elapsedTime = TimeStepSys;
        ort->timeInYear += elapsedTime;
        for (iInObj = 1; iInObj <= ort->OutputTableBinnedCount; ++iInObj) {
            // get values of array for current object being referenced
            curIntervalStart = ort->OutputTableBinned(iInObj).intervalStart;
            curIntervalSize = ort->OutputTableBinned(iInObj).intervalSize;
            curIntervalCount = ort->OutputTableBinned(iInObj).intervalCount;
            curResIndex = ort->OutputTableBinned(iInObj).resIndex;
            curNumTables = ort->OutputTableBinned(iInObj).numTables;
            topValue = curIntervalStart + curIntervalSize * curIntervalCount;
            curTypeOfVar = ort->OutputTableBinned(iInObj).typeOfVar;
            curStepType = ort->OutputTableBinned(iInObj).stepType;
            curScheduleIndex = ort->OutputTableBinned(iInObj).scheduleIndex;
            // if a schedule was used, check if it was non-zero value
            if (curScheduleIndex != 0) {
                if (GetCurrentScheduleValue(state, curScheduleIndex) != 0.0) {
                    gatherThisTime = true;
                } else {
                    gatherThisTime = false;
                }
            } else {
                gatherThisTime = true;
            }
            if (gatherThisTime) {
                for (jTable = 1; jTable <= curNumTables; ++jTable) {
                    repIndex = curResIndex + (jTable - 1);
                    if (((curStepType == OutputProcessor::TimeStepType::TimeStepZone) &&
                         (t_timeStepType == OutputProcessor::TimeStepType::TimeStepZone)) ||
                        ((curStepType == OutputProcessor::TimeStepType::TimeStepSystem) &&
                         (t_timeStepType == OutputProcessor::TimeStepType::TimeStepSystem))) {
                        // put actual value from OutputProcesser arrays
                        curValue = GetInternalVariableValue(state, curTypeOfVar, ort->BinObjVarID(repIndex).varMeterNum);
                        // per MJW when a summed variable is used divide it by the length of the time step
                        if (t_timeStepType == OutputProcessor::TimeStepType::TimeStepSystem) {
                            elapsedTime = TimeStepSys;
                        } else {
                            elapsedTime = state.dataGlobal->TimeStepZone;
                        }
                        if (ort->OutputTableBinned(iInObj).avgSum == OutputProcessor::StoreType::Summed) { // if it is a summed variable
                            curValue /= (elapsedTime * DataGlobalConstants::SecInHour);
                        }
                        // round the value to the number of signficant digits used in the final output report
                        if (curIntervalSize < 1) {
                            curValue = round(curValue * 10000.0) / 10000.0; // four significant digits
                        } else if (curIntervalSize >= 10) {
                            curValue = round(curValue); // zero significant digits
                        } else {
                            curValue = round(curValue * 100.0) / 100.0; // two significant digits
                        }
                        // check if the value is above the maximum or below the minimum value
                        // first before binning the value within the range.
                        if (curValue < curIntervalStart) {
                            ort->BinResultsBelow(repIndex).mnth(state.dataEnvrn->Month) += elapsedTime;
                            ort->BinResultsBelow(repIndex).hrly(state.dataGlobal->HourOfDay) += elapsedTime;
                        } else if (curValue >= topValue) {
                            ort->BinResultsAbove(repIndex).mnth(state.dataEnvrn->Month) += elapsedTime;
                            ort->BinResultsAbove(repIndex).hrly(state.dataGlobal->HourOfDay) += elapsedTime;
                        } else {
                            // determine which bin the results are in
                            binNum = int((curValue - curIntervalStart) / curIntervalSize) + 1;
                            ort->BinResults(binNum, repIndex).mnth(state.dataEnvrn->Month) += elapsedTime;
                            ort->BinResults(binNum, repIndex).hrly(state.dataGlobal->HourOfDay) += elapsedTime;
                        }
                        // add to statistics array
                        ++ort->BinStatistics(repIndex).n;
                        ort->BinStatistics(repIndex).sum += curValue;
                        ort->BinStatistics(repIndex).sum2 += curValue * curValue;
                        if (curValue < ort->BinStatistics(repIndex).minimum) {
                            ort->BinStatistics(repIndex).minimum = curValue;
                        }
                        if (curValue > ort->BinStatistics(repIndex).maximum) {
                            ort->BinStatistics(repIndex).maximum = curValue;
                        }
                    }
                }
            }
        }
    }

    void GatherMonthlyResultsForTimestep(EnergyPlusData &state, OutputProcessor::TimeStepType t_timeStepType) // What kind of data to update (Zone, HVAC)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   September 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Gathers the data each timestep and updates the arrays
        //   holding the data that will be reported later.

        // Using/Aliasing
        using DataHVACGlobals::TimeStepSys;
        using General::DetermineMinuteForReporting;
        using General::EncodeMonDayHrMin;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int iTable;  // loop variable for monthlyTables
        int jColumn; // loop variable for monthlyColumns
        int curCol;
        Real64 curValue;
        int curTypeOfVar;
        int curVarNum;
        Real64 elapsedTime;
        Real64 oldResultValue;
        int oldTimeStamp;
        Real64 oldDuration;
        Real64 newResultValue;
        int newTimeStamp;
        Real64 newDuration;
        int timestepTimeStamp;
        bool activeMinMax;
        // LOGICAL,SAVE  :: activeHoursShown=.FALSE.  !fix by LKL addressing CR6482
        bool activeHoursShown;
        bool activeNewValue;
        OutputProcessor::TimeStepType curStepType;
        int minuteCalculated;
        int kOtherColumn; // variable used in loop to scan through additional columns
        int scanColumn;
        Real64 scanValue;
        int scanTypeOfVar;
        int scanVarNum;
        Real64 oldScanValue;
        // local copies of some of the MonthlyColumns array references since
        // profiling showed that they were slow.

        static Array1D_int MonthlyColumnsTypeOfVar;
        static Array1D<OutputProcessor::TimeStepType> MonthlyColumnsStepType;
        static Array1D<iAggType> MonthlyColumnsAggType;
        static Array1D_int MonthlyColumnsVarNum;
        static Array1D_int MonthlyTablesNumColumns;
        static int curFirstColumn(0);

        if (!state.dataGlobal->DoWeathSim) return;
        auto &ort(state.dataOutRptTab);
        assert(state.dataGlobal->TimeStepZoneSec > 0.0);

        // create temporary arrays to speed processing of these arrays
        if (ort->GatherMonthlyResultsForTimestepRunOnce) {
            // MonthlyColumns
            MonthlyColumnsTypeOfVar.allocate(ort->MonthlyColumns.I());
            MonthlyColumnsStepType.allocate(ort->MonthlyColumns.I());
            MonthlyColumnsAggType.allocate(ort->MonthlyColumns.I());
            MonthlyColumnsVarNum.allocate(ort->MonthlyColumns.I());
            for (int i = ort->MonthlyColumns.l(), e = ort->MonthlyColumns.u(); i <= e; ++i) {
                auto const &col(ort->MonthlyColumns(i));
                MonthlyColumnsTypeOfVar(i) = col.typeOfVar;
                MonthlyColumnsStepType(i) = col.stepType;
                MonthlyColumnsAggType(i) = col.aggType;
                MonthlyColumnsVarNum(i) = col.varNum;
            }

            // MonthlyTables
            MonthlyTablesNumColumns.allocate(ort->MonthlyTables.I());
            for (int i = ort->MonthlyTables.l(), e = ort->MonthlyTables.u(); i <= e; ++i) {
                MonthlyTablesNumColumns(i) = ort->MonthlyTables(i).numColumns;
            }

            // set flag so this block is only executed once
            ort->GatherMonthlyResultsForTimestepRunOnce = false;
        }

        elapsedTime = TimeStepSys;
        if (t_timeStepType == OutputProcessor::TimeStepType::TimeStepSystem) {
            elapsedTime = TimeStepSys;
        } else {
            elapsedTime = state.dataGlobal->TimeStepZone;
        }
        ort->IsMonthGathered(state.dataEnvrn->Month) = true;
        for (iTable = 1; iTable <= ort->MonthlyTablesCount; ++iTable) {
            activeMinMax = false;     // at the beginning of the new timestep
            activeHoursShown = false; // fix by JG addressing CR6482
            curFirstColumn = ort->MonthlyTables(iTable).firstColumn;
            for (jColumn = 1; jColumn <= MonthlyTablesNumColumns(iTable); ++jColumn) {
                curCol = jColumn + curFirstColumn - 1;
                curTypeOfVar = MonthlyColumnsTypeOfVar(curCol);
                curStepType = MonthlyColumnsStepType(curCol);
                if (((curStepType == OutputProcessor::TimeStepType::TimeStepZone) &&
                     (t_timeStepType == OutputProcessor::TimeStepType::TimeStepZone)) ||
                    ((curStepType == OutputProcessor::TimeStepType::TimeStepSystem) &&
                     (t_timeStepType == OutputProcessor::TimeStepType::TimeStepSystem))) {
                    //  the above condition used to include the following prior to new scan method
                    //  (MonthlyColumns(curCol)%aggType .EQ. iAggType::ValueWhenMaxMin)
                    curVarNum = MonthlyColumnsVarNum(curCol);
                    curValue = GetInternalVariableValue(state, curTypeOfVar, curVarNum);
                    // Get the value from the result array
                    oldResultValue = ort->MonthlyColumns(curCol).reslt(state.dataEnvrn->Month);
                    oldTimeStamp = ort->MonthlyColumns(curCol).timeStamp(state.dataEnvrn->Month);
                    oldDuration = ort->MonthlyColumns(curCol).duration(state.dataEnvrn->Month);
                    // Zero the revised values (as default if not set later in SELECT)
                    newResultValue = 0.0;
                    newTimeStamp = 0;
                    newDuration = 0.0;
                    activeNewValue = false;
                    // the current timestamp
                    minuteCalculated = DetermineMinuteForReporting(state, t_timeStepType);
                    //      minuteCalculated = (CurrentTime - INT(CurrentTime))*60
                    //      IF (t_timeStepType .EQ. OutputProcessor::TimeStepType::TimeStepSystem) minuteCalculated = minuteCalculated +
                    //      SysTimeElapsed * 60 minuteCalculated = INT((TimeStep-1) * TimeStepZone * 60) + INT((SysTimeElapsed + TimeStepSys) * 60)
                    EncodeMonDayHrMin(timestepTimeStamp, state.dataEnvrn->Month, state.dataEnvrn->DayOfMonth, state.dataGlobal->HourOfDay, minuteCalculated);
                    // perform the selected aggregation type
                    // use next lines since it is faster was: SELECT CASE (MonthlyColumns(curCol)%aggType)
                    {
                        auto const SELECT_CASE_var(MonthlyColumnsAggType(curCol));
                        if (SELECT_CASE_var == iAggType::SumOrAvg) {
                            if (ort->MonthlyColumns(curCol).avgSum == OutputProcessor::StoreType::Summed) { // if it is a summed variable
                                newResultValue = oldResultValue + curValue;
                            } else {
                                newResultValue = oldResultValue + curValue * elapsedTime; // for averaging - weight by elapsed time
                            }
                            newDuration = oldDuration + elapsedTime;
                            activeNewValue = true;
                        } else if (SELECT_CASE_var == iAggType::Maximum) {
                            // per MJW when a summed variable is used divide it by the length of the time step
                            if (ort->MonthlyColumns(curCol).avgSum == OutputProcessor::StoreType::Summed) { // if it is a summed variable
                                if (t_timeStepType == OutputProcessor::TimeStepType::TimeStepSystem) {
                                    curValue /= (TimeStepSys * DataGlobalConstants::SecInHour);
                                } else {
                                    curValue /= state.dataGlobal->TimeStepZoneSec;
                                }
                            }
                            if (curValue > oldResultValue) {
                                newResultValue = curValue;
                                newTimeStamp = timestepTimeStamp;
                                activeMinMax = true;
                                activeNewValue = true;
                            } else {
                                activeMinMax = false; // reset this
                            }
                        } else if (SELECT_CASE_var == iAggType::Minimum) {
                            // per MJW when a summed variable is used divide it by the length of the time step
                            if (ort->MonthlyColumns(curCol).avgSum == OutputProcessor::StoreType::Summed) { // if it is a summed variable
                                if (t_timeStepType == OutputProcessor::TimeStepType::TimeStepSystem) {
                                    curValue /= (TimeStepSys * DataGlobalConstants::SecInHour);
                                } else {
                                    curValue /= state.dataGlobal->TimeStepZoneSec;
                                }
                            }
                            if (curValue < oldResultValue) {
                                newResultValue = curValue;
                                newTimeStamp = timestepTimeStamp;
                                activeMinMax = true;
                                activeNewValue = true;
                            } else {
                                activeMinMax = false; // reset this
                            }
                        } else if (SELECT_CASE_var == iAggType::HoursZero) {
                            if (curValue == 0) {
                                newResultValue = oldResultValue + elapsedTime;
                                activeHoursShown = true;
                                activeNewValue = true;
                            } else {
                                activeHoursShown = false;
                            }
                        } else if (SELECT_CASE_var == iAggType::HoursNonZero) {
                            if (curValue != 0) {
                                newResultValue = oldResultValue + elapsedTime;
                                activeHoursShown = true;
                                activeNewValue = true;
                            } else {
                                activeHoursShown = false;
                            }
                        } else if (SELECT_CASE_var == iAggType::HoursPositive) {
                            if (curValue > 0) {
                                newResultValue = oldResultValue + elapsedTime;
                                activeHoursShown = true;
                                activeNewValue = true;
                            } else {
                                activeHoursShown = false;
                            }
                        } else if (SELECT_CASE_var == iAggType::HoursNonPositive) {
                            if (curValue <= 0) {
                                newResultValue = oldResultValue + elapsedTime;
                                activeHoursShown = true;
                                activeNewValue = true;
                            } else {
                                activeHoursShown = false;
                            }
                        } else if (SELECT_CASE_var == iAggType::HoursNegative) {
                            if (curValue < 0) {
                                newResultValue = oldResultValue + elapsedTime;
                                activeHoursShown = true;
                                activeNewValue = true;
                            } else {
                                activeHoursShown = false;
                            }
                        } else if (SELECT_CASE_var == iAggType::HoursNonNegative) {
                            if (curValue >= 0) {
                                newResultValue = oldResultValue + elapsedTime;
                                activeHoursShown = true;
                                activeNewValue = true;
                            } else {
                                activeHoursShown = false;
                            }
                            // The valueWhenMaxMin is picked up now during the activeMinMax if block below.
                            // CASE (iAggType::ValueWhenMaxMin)
                            // CASE (iAggType::SumOrAverageHoursShown)
                            // CASE (iAggType::MaximumDuringHoursShown)
                            // CASE (iAggType::MinimumDuringHoursShown)
                        }
                    }
                    // if the new value has been set then set the monthly values to the
                    // new columns. This skips the aggregation types that don't even get
                    // triggered now such as valueWhenMinMax and all the agg*HoursShown
                    if (activeNewValue) {
                        ort->MonthlyColumns(curCol).reslt(state.dataEnvrn->Month) = newResultValue;
                        ort->MonthlyColumns(curCol).timeStamp(state.dataEnvrn->Month) = newTimeStamp;
                        ort->MonthlyColumns(curCol).duration(state.dataEnvrn->Month) = newDuration;
                    }
                    // if a minimum or maximum value was set this timeStep then
                    // scan the remaining columns of the table looking for values
                    // that are aggregation type "ValueWhenMaxMin" and set their values
                    // if another minimum or maximum column is found then end
                    // the scan (it will be taken care of when that column is done)
                    if (activeMinMax) {
                        for (kOtherColumn = jColumn + 1; kOtherColumn <= ort->MonthlyTables(iTable).numColumns; ++kOtherColumn) {
                            scanColumn = kOtherColumn + ort->MonthlyTables(iTable).firstColumn - 1;
                            {
                                auto const SELECT_CASE_var(ort->MonthlyColumns(scanColumn).aggType);
                                if ((SELECT_CASE_var == iAggType::Maximum) || (SELECT_CASE_var == iAggType::Minimum)) {
                                    // end scanning since these might reset
                                    break; // do
                                } else if (SELECT_CASE_var == iAggType::ValueWhenMaxMin) {
                                    // this case is when the value should be set
                                    scanTypeOfVar = ort->MonthlyColumns(scanColumn).typeOfVar;
                                    scanVarNum = ort->MonthlyColumns(scanColumn).varNum;
                                    scanValue = GetInternalVariableValue(state, scanTypeOfVar, scanVarNum);
                                    // When a summed variable is used divide it by the length of the time step
                                    if (ort->MonthlyColumns(scanColumn).avgSum == OutputProcessor::StoreType::Summed) { // if it is a summed variable
                                        if (t_timeStepType == OutputProcessor::TimeStepType::TimeStepSystem) {
                                            scanValue /= (TimeStepSys * DataGlobalConstants::SecInHour);
                                        } else {
                                            scanValue /= state.dataGlobal->TimeStepZoneSec;
                                        }
                                    }
                                    ort->MonthlyColumns(scanColumn).reslt(state.dataEnvrn->Month) = scanValue;
                                } else {
                                    // do nothing
                                }
                            }
                        }
                    }
                    // If the hours variable is active then scan through the rest of the variables
                    // and accumulate
                    if (activeHoursShown) {
                        for (kOtherColumn = jColumn + 1; kOtherColumn <= ort->MonthlyTables(iTable).numColumns; ++kOtherColumn) {
                            scanColumn = kOtherColumn + ort->MonthlyTables(iTable).firstColumn - 1;
                            scanTypeOfVar = ort->MonthlyColumns(scanColumn).typeOfVar;
                            scanVarNum = ort->MonthlyColumns(scanColumn).varNum;
                            scanValue = GetInternalVariableValue(state, scanTypeOfVar, scanVarNum);
                            oldScanValue = ort->MonthlyColumns(scanColumn).reslt(state.dataEnvrn->Month);
                            {
                                auto const SELECT_CASE_var(ort->MonthlyColumns(scanColumn).aggType);
                                if ((SELECT_CASE_var == iAggType::HoursZero) || (SELECT_CASE_var == iAggType::HoursNonZero)) {
                                    // end scanning since these might reset
                                    break; // do
                                } else if ((SELECT_CASE_var == iAggType::HoursPositive) || (SELECT_CASE_var == iAggType::HoursNonPositive)) {
                                    // end scanning since these might reset
                                    break; // do
                                } else if ((SELECT_CASE_var == iAggType::HoursNegative) || (SELECT_CASE_var == iAggType::HoursNonNegative)) {
                                    // end scanning since these might reset
                                    break; // do
                                } else if (SELECT_CASE_var == iAggType::SumOrAverageHoursShown) {
                                    // this case is when the value should be set
                                    if (ort->MonthlyColumns(scanColumn).avgSum == OutputProcessor::StoreType::Summed) { // if it is a summed variable
                                        ort->MonthlyColumns(scanColumn).reslt(state.dataEnvrn->Month) = oldScanValue + scanValue;
                                    } else {
                                        // for averaging - weight by elapsed time
                                        ort->MonthlyColumns(scanColumn).reslt(state.dataEnvrn->Month) = oldScanValue + scanValue * elapsedTime;
                                    }
                                    ort->MonthlyColumns(scanColumn).duration(state.dataEnvrn->Month) += elapsedTime;
                                } else if (SELECT_CASE_var == iAggType::MaximumDuringHoursShown) {
                                    if (ort->MonthlyColumns(scanColumn).avgSum == OutputProcessor::StoreType::Summed) { // if it is a summed variable
                                        if (t_timeStepType == OutputProcessor::TimeStepType::TimeStepSystem) {
                                            scanValue /= (TimeStepSys * DataGlobalConstants::SecInHour);
                                        } else {
                                            scanValue /= state.dataGlobal->TimeStepZoneSec;
                                        }
                                    }
                                    if (scanValue > oldScanValue) {
                                        ort->MonthlyColumns(scanColumn).reslt(state.dataEnvrn->Month) = scanValue;
                                        ort->MonthlyColumns(scanColumn).timeStamp(state.dataEnvrn->Month) = timestepTimeStamp;
                                    }
                                } else if (SELECT_CASE_var == iAggType::MinimumDuringHoursShown) {
                                    if (ort->MonthlyColumns(scanColumn).avgSum == OutputProcessor::StoreType::Summed) { // if it is a summed variable
                                        if (t_timeStepType == OutputProcessor::TimeStepType::TimeStepSystem) {
                                            scanValue /= (TimeStepSys * DataGlobalConstants::SecInHour);
                                        } else {
                                            scanValue /= state.dataGlobal->TimeStepZoneSec;
                                        }
                                    }
                                    if (scanValue < oldScanValue) {
                                        ort->MonthlyColumns(scanColumn).reslt(state.dataEnvrn->Month) = scanValue;
                                        ort->MonthlyColumns(scanColumn).timeStamp(state.dataEnvrn->Month) = timestepTimeStamp;
                                    }
                                } else {
                                    // do nothing
                                }
                            }
                            activeHoursShown = false; // fixed CR8317
                        }
                    }
                }
            }
        }
    }

    void GatherBEPSResultsForTimestep(EnergyPlusData &state, OutputProcessor::TimeStepType t_timeStepType) // What kind of data to update (Zone, HVAC)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   November 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   This routine gathers data for producing the BEPS report

        // METHODOLOGY EMPLOYED:
        //   Uses get input structure similar to other objects
        //   Meter names are of two forms:
        //         <ResourceType>:<name>
        //   or
        //         <EndUseType>:<ResourceType>
        //   For the purposes of this routine, only the facility <name>
        //   is used.  Remember that 'Building' is actually the sum of
        //   the zones only without system,plant and exterior. The only
        //   way to get them all is to use 'facility'
        //   The <EndUseType> are:
        //          Heating
        //          Cooling
        //          InteriorLights
        //          ExteriorLights
        //          InteriorEquipment
        //          ExteriorEquipment
        //          Fans
        //          Pumps
        //          HeatRejection
        //          Humidifier
        //          HeatRecovery
        //          DHW
        //          Refrigeration
        //          Cogeneration
        //   The <ResourceType> are:
        //          Electricity
        //          Gas
        //          Gasoline
        //          Diesel
        //          Coal
        //          FuelOilNo1
        //          FuelOilNo2
        //          Propane
        //          Water
        //          Steam
        //          DistrictCooling
        //          DistrictHeating

        // REFERENCES:
        // na

        // Using/Aliasing
        using DataStringGlobals::CharComma;
        using DataStringGlobals::CharSpace;
        using DataStringGlobals::CharTab;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int iResource;
        int kEndUseSub;
        Real64 curMeterValue;
        int curMeterNumber;
        auto &ort(state.dataOutRptTab);

        // if no beps report is called then skip

        if ((ort->displayTabularBEPS || ort->displayLEEDSummary) && (t_timeStepType == OutputProcessor::TimeStepType::TimeStepZone)) {
            // add the current time to the total elapsed time
            // FOLLOWING LINE MOVED TO UPDATETABULARREPORTS because used even when beps is not called
            // gatherElapsedTimeBEPS = gatherElapsedTimeBEPS + TimeStepZone
            // loop through all of the resource types for the entire facility
            //  DO iResource = 1, numResourceTypes
            //    curMeterNumber = meterNumTotalsBEPS(iResource)
            //    IF (curMeterNumber .GT. 0) THEN
            //      curMeterValue = GetCurrentMeterValue(curMeterNumber)
            //      gatherTotalsBEPS(iResource) = gatherTotalsBEPS(iResource) + curMeterValue
            //    END IF
            //  END DO

            // loop through all of the resources and end uses for the entire facility
            for (iResource = 1; iResource <= numResourceTypes; ++iResource) {
                curMeterNumber = ort->meterNumTotalsBEPS(iResource);
                if (curMeterNumber > 0) {
                    curMeterValue = GetCurrentMeterValue(state, curMeterNumber);
                    ort->gatherTotalsBEPS(iResource) += curMeterValue;
                }

                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    curMeterNumber = ort->meterNumEndUseBEPS(iResource, jEndUse);
                    if (curMeterNumber > 0) {
                        curMeterValue = GetCurrentMeterValue(state, curMeterNumber);
                        ort->gatherEndUseBEPS(iResource, jEndUse) += curMeterValue;

                        for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                            curMeterNumber = ort->meterNumEndUseSubBEPS(kEndUseSub, jEndUse, iResource);
                            if (curMeterNumber > 0) {
                                curMeterValue = GetCurrentMeterValue(state, curMeterNumber);
                                ort->gatherEndUseSubBEPS(kEndUseSub, jEndUse, iResource) += curMeterValue;
                            }
                        }
                    }
                }
            }

            for (iResource = 1; iResource <= numSourceTypes; ++iResource) {
                curMeterNumber = ort->meterNumTotalsSource(iResource);
                if (curMeterNumber > 0) {
                    curMeterValue = GetCurrentMeterValue(state, curMeterNumber);
                    ort->gatherTotalsSource(iResource) += curMeterValue;
                }
            }

            // gather the electric load components
            ort->gatherPowerFuelFireGen += GetCurrentMeterValue(state, ort->meterNumPowerFuelFireGen);
            ort->gatherPowerPV += GetCurrentMeterValue(state, ort->meterNumPowerPV);
            ort->gatherPowerWind += GetCurrentMeterValue(state, ort->meterNumPowerWind);
            ort->gatherPowerHTGeothermal += GetCurrentMeterValue(state, ort->meterNumPowerHTGeothermal);
            ort->gatherElecProduced += GetCurrentMeterValue(state, ort->meterNumElecProduced);
            ort->gatherElecPurchased += GetCurrentMeterValue(state, ort->meterNumElecPurchased);
            ort->gatherElecSurplusSold += GetCurrentMeterValue(state, ort->meterNumElecSurplusSold);
            ort->gatherElecStorage += GetCurrentMeterValue(state, ort->meterNumElecStorage);
            ort->gatherPowerConversion += GetCurrentMeterValue(state, ort->meterNumPowerConversion);
            // gather the onsite thermal components
            ort->gatherWaterHeatRecovery += GetCurrentMeterValue(state, ort->meterNumWaterHeatRecovery);
            ort->gatherAirHeatRecoveryCool += GetCurrentMeterValue(state, ort->meterNumAirHeatRecoveryCool);
            ort->gatherAirHeatRecoveryHeat += GetCurrentMeterValue(state, ort->meterNumAirHeatRecoveryHeat);
            ort->gatherHeatHTGeothermal += GetCurrentMeterValue(state, ort->meterNumHeatHTGeothermal);
            ort->gatherHeatSolarWater += GetCurrentMeterValue(state, ort->meterNumHeatSolarWater);
            ort->gatherHeatSolarAir += GetCurrentMeterValue(state, ort->meterNumHeatSolarAir);
            // gather the water supply components
            ort->gatherRainWater += GetCurrentMeterValue(state, ort->meterNumRainWater);
            ort->gatherCondensate += GetCurrentMeterValue(state, ort->meterNumCondensate);
            ort->gatherWellwater += GetCurrentMeterValue(state, ort->meterNumGroundwater);
            ort->gatherMains += GetCurrentMeterValue(state, ort->meterNumMains);
            ort->gatherWaterEndUseTotal += GetCurrentMeterValue(state, ort->meterNumWaterEndUseTotal);
        }
    }

    void GatherSourceEnergyEndUseResultsForTimestep(EnergyPlusData &state, OutputProcessor::TimeStepType t_timeStepType) // What kind of data to update (Zone, HVAC)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Mangesh Basarkar
        //       DATE WRITTEN   September 2011
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   This routine gathers data for producing the end uses report in source energy

        // METHODOLOGY EMPLOYED:
        //   Uses get input structure similar to other objects
        //   Meter names are of two forms:
        //         <ResourceType>:<name>
        //   or
        //         <EndUseType>:<ResourceType>
        //   The <EndUseType> are:
        //          Heating
        //          Cooling
        //          InteriorLights
        //          ExteriorLights
        //          InteriorEquipment
        //          ExteriorEquipment
        //          Fans
        //          Pumps
        //          HeatRejection
        //          Humidifier
        //          HeatRecovery
        //          DHW
        //          Refrigeration
        //          Cogeneration
        //   The <ResourceType> are:
        //          Electricity 1
        //          Gas 2
        //          Gasoline 6
        //          Diesel 8
        //          Coal 9
        //          FuelOilNo1 10
        //          FuelOilNo2 11
        //          Propane 12
        //          Water 7
        //          Steam 5
        //          DistrictCooling 3
        //          DistrictHeating 4

        //          sourceTypeNames(1)='Electric'
        //          sourceTypeNames(2)='NaturalGas'
        //          sourceTypeNames(3)='Gasoline'
        //          sourceTypeNames(4)='Diesel'
        //          sourceTypeNames(5)='Coal'
        //          sourceTypeNames(6)='FuelOilNo1'
        //          sourceTypeNames(7)='FuelOilNo2'
        //          sourceTypeNames(8)='Propane'
        //          sourceTypeNames(9)='PurchasedElectric'
        //          sourceTypeNames(10)='SoldElectric'
        //          sourceTypeNames(11)='OtherFuel1'
        //          sourceTypeNames(12)='OtherFuel2'

        // REFERENCES:
        // na

        // Using/Aliasing
        using DataStringGlobals::CharComma;
        using DataStringGlobals::CharSpace;
        using DataStringGlobals::CharTab;
        using ScheduleManager::GetCurrentScheduleValue;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int iResource;
        Real64 curMeterValue;
        int curMeterNumber;
        auto &ort(state.dataOutRptTab);

        // if no beps by source report is called then skip

        if ((ort->displaySourceEnergyEndUseSummary) && (t_timeStepType == OutputProcessor::TimeStepType::TimeStepZone)) {
            // loop through all of the resources and end uses for the entire facility
            for (iResource = 1; iResource <= numResourceTypes; ++iResource) {

                if (ort->ffSchedUsed(iResource)) {
                    curMeterNumber = ort->meterNumTotalsBEPS(iResource);
                    if (curMeterNumber > 0) {
                        curMeterValue =
                            GetCurrentMeterValue(state, curMeterNumber) * GetCurrentScheduleValue(state, ort->ffSchedIndex(iResource)) * ort->SourceFactors(iResource);
                        ort->gatherTotalsBySourceBEPS(iResource) += curMeterValue;
                    }
                } else {
                    curMeterNumber = ort->meterNumTotalsBEPS(iResource);
                    if (curMeterNumber > 0) {
                        curMeterValue = GetCurrentMeterValue(state, curMeterNumber) * ort->SourceFactors(iResource);
                        ort->gatherTotalsBySourceBEPS(iResource) += curMeterValue;
                    }
                }

                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    if (ort->ffSchedUsed(iResource)) {
                        curMeterNumber = ort->meterNumEndUseBEPS(iResource, jEndUse);
                        if (curMeterNumber > 0) {
                            curMeterValue =
                                GetCurrentMeterValue(state, curMeterNumber) * GetCurrentScheduleValue(state, ort->ffSchedIndex(iResource)) * ort->SourceFactors(iResource);
                            ort->gatherEndUseBySourceBEPS(iResource, jEndUse) += curMeterValue;
                        }
                    } else {
                        curMeterNumber = ort->meterNumEndUseBEPS(iResource, jEndUse);
                        if (curMeterNumber > 0) {
                            curMeterValue = GetCurrentMeterValue(state, curMeterNumber) * ort->SourceFactors(iResource);
                            ort->gatherEndUseBySourceBEPS(iResource, jEndUse) += curMeterValue;
                        }
                    }
                }
            }
        }
    }

    void GatherPeakDemandForTimestep(EnergyPlusData &state, OutputProcessor::TimeStepType t_timeStepType) // What kind of data to update (Zone, HVAC)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   January 2009
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   This routine gathers data for producing the Peak Demand
        //   by end-use report

        // METHODOLOGY EMPLOYED:
        //   Uses get input structure similar to other objects
        //   Meter names are of two forms:
        //         <ResourceType>:<name>
        //   or
        //         <EndUseType>:<ResourceType>
        //   For the purposes of this routine, only the facility <name>
        //   is used.  Remember that 'Building' is actually the sum of
        //   the zones only without system,plant and exterior. The only
        //   way to get them all is to use 'facility'
        //   The <EndUseType> are:
        //          Heating
        //          Cooling
        //          InteriorLights
        //          ExteriorLights
        //          InteriorEquipment
        //          ExteriorEquipment
        //          Fans
        //          Pumps
        //          HeatRejection
        //          Humidifier
        //          HeatRecovery
        //          DHW
        //          Refrigeration
        //          Cogeneration
        //   The <ResourceType> are:
        //          Electricity
        //          Gas
        //          Gasoline
        //          Diesel
        //          Coal
        //          FuelOilNo1
        //          FuelOilNo2
        //          Propane
        //          Water
        //          Steam
        //          DistrictCooling
        //          DistrictHeating

        // REFERENCES:
        // na

        // Using/Aliasing
        using DataStringGlobals::CharComma;
        using DataStringGlobals::CharSpace;
        using DataStringGlobals::CharTab;
        using General::DetermineMinuteForReporting;
        using General::EncodeMonDayHrMin;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int iResource;
        int kEndUseSub;
        Real64 curDemandValue;
        int curMeterNumber;
        int minuteCalculated;
        int timestepTimeStamp;
        assert(state.dataGlobal->TimeStepZoneSec > 0.0);
        auto &ort(state.dataOutRptTab);

        if ((ort->displayDemandEndUse) && (t_timeStepType == OutputProcessor::TimeStepType::TimeStepZone)) {
            // loop through all of the resources and end uses for the entire facility
            for (iResource = 1; iResource <= numResourceTypes; ++iResource) {
                curMeterNumber = ort->meterNumTotalsBEPS(iResource);
                if (curMeterNumber > 0) {
                    curDemandValue = GetCurrentMeterValue(state, curMeterNumber) / state.dataGlobal->TimeStepZoneSec;
                    // check if current value is greater than existing peak demand value
                    if (curDemandValue > ort->gatherDemandTotal(iResource)) {
                        ort->gatherDemandTotal(iResource) = curDemandValue;
                        // save the time that the peak demand occurred
                        //        minuteCalculated = (CurrentTime - INT(CurrentTime))*60
                        minuteCalculated = DetermineMinuteForReporting(state, t_timeStepType);
                        EncodeMonDayHrMin(timestepTimeStamp, state.dataEnvrn->Month, state.dataEnvrn->DayOfMonth, state.dataGlobal->HourOfDay, minuteCalculated);
                        ort->gatherDemandTimeStamp(iResource) = timestepTimeStamp;
                        // if new peak demand is set, then gather all of the end use values at this particular
                        // time to find the components of the peak demand
                        for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                            curMeterNumber = ort->meterNumEndUseBEPS(iResource, jEndUse);
                            if (curMeterNumber > 0) {
                                curDemandValue = GetCurrentMeterValue(state, curMeterNumber) / state.dataGlobal->TimeStepZoneSec;
                                ort->gatherDemandEndUse(iResource, jEndUse) = curDemandValue;
                                for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                                    curMeterNumber = ort->meterNumEndUseSubBEPS(kEndUseSub, jEndUse, iResource);
                                    if (curMeterNumber > 0) {
                                        curDemandValue = GetCurrentMeterValue(state, curMeterNumber) / state.dataGlobal->TimeStepZoneSec;
                                        ort->gatherDemandEndUseSub(kEndUseSub, jEndUse, iResource) = curDemandValue;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // gather the peak demands of each individual enduse subcategory for the LEED report
        if ((ort->displayLEEDSummary) && (t_timeStepType == OutputProcessor::TimeStepType::TimeStepZone)) {
            // loop through all of the resources and end uses for the entire facility
            for (iResource = 1; iResource <= numResourceTypes; ++iResource) {
                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    curMeterNumber = ort->meterNumEndUseBEPS(iResource, jEndUse);
                    if (curMeterNumber > 0) {
                        curDemandValue = GetCurrentMeterValue(state, curMeterNumber) / state.dataGlobal->TimeStepZoneSec;
                        if (curDemandValue > ort->gatherDemandIndEndUse(iResource, jEndUse)) {
                            ort->gatherDemandIndEndUse(iResource, jEndUse) = curDemandValue;
                        }
                        for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                            curMeterNumber = ort->meterNumEndUseSubBEPS(kEndUseSub, jEndUse, iResource);
                            if (curMeterNumber > 0) {
                                curDemandValue = GetCurrentMeterValue(state, curMeterNumber) / state.dataGlobal->TimeStepZoneSec;
                                // check if current value is greater than existing peak demand value
                                if (curDemandValue > ort->gatherDemandIndEndUseSub(kEndUseSub, jEndUse, iResource)) {
                                    ort->gatherDemandIndEndUseSub(kEndUseSub, jEndUse, iResource) = curDemandValue;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void GatherHeatEmissionReport(EnergyPlusData &state, OutputProcessor::TimeStepType t_timeStepType)
    {
        // PURPOSE OF THIS SUBROUTINE:
        // Gathers the data each zone timestep for the heat gain report.
        // The routine generates an annual table with the following columns which correspond to
        // the output variables and data structures shown.

        // Using/Aliasing
        using DataHeatBalance::BuildingPreDefRep;

        using DataHeatBalance::ZoneTotalExfiltrationHeatLoss;
        using DataHeatBalance::ZoneTotalExhaustHeatLoss;
        using DataHeatBalSurface::SumSurfaceHeatEmission;

        using DataHVACGlobals::TimeStepSys;

        SysTotalHVACReliefHeatLoss = 0;
        SysTotalHVACRejectHeatLoss = 0;
        auto &ort(state.dataOutRptTab);

        if (!ort->displayHeatEmissionsSummary) return; // don't gather data if report isn't requested

        // Only gather zone report at zone time steps
        if (t_timeStepType == OutputProcessor::TimeStepType::TimeStepZone) {
            BuildingPreDefRep.emiEnvelopConv += SumSurfaceHeatEmission * DataGlobalConstants::convertJtoGJ;
            return;
        }

        CalcHeatEmissionReport(state);
        BuildingPreDefRep.emiZoneExfiltration += ZoneTotalExfiltrationHeatLoss * DataGlobalConstants::convertJtoGJ;
        BuildingPreDefRep.emiZoneExhaust += ZoneTotalExhaustHeatLoss * DataGlobalConstants::convertJtoGJ;
        BuildingPreDefRep.emiHVACRelief += SysTotalHVACReliefHeatLoss * DataGlobalConstants::convertJtoGJ;
        BuildingPreDefRep.emiHVACReject += SysTotalHVACRejectHeatLoss * DataGlobalConstants::convertJtoGJ;

        BuildingPreDefRep.emiTotHeat = BuildingPreDefRep.emiEnvelopConv + BuildingPreDefRep.emiZoneExfiltration + BuildingPreDefRep.emiZoneExhaust +
                                       BuildingPreDefRep.emiHVACRelief + BuildingPreDefRep.emiHVACReject;
    }


    void CalcHeatEmissionReport(EnergyPlusData &state)
    {
        // PURPOSE OF THIS SUBROUTINE:
        // Gathers the data each zone timestep for the heat gain report.
        // The routine generates an annual table with the following columns which correspond to
        // the output variables and data structures shown.

        // Using/Aliasing
        using DataHeatBalance::BuildingPreDefRep;
        using DataHeatBalance::NumRefrigCondensers;
        using DataHeatBalance::NumRefrigeratedRacks;
        using DataHeatBalance::SysTotalHVACRejectHeatLoss;
        using DataHeatBalance::SysTotalHVACReliefHeatLoss;
        using DataHeatBalance::ZoneTotalExfiltrationHeatLoss;
        using DataHeatBalance::ZoneTotalExhaustHeatLoss;
        using DataHVACGlobals::AirCooled;
        using DataHVACGlobals::EvapCooled;
        using DataHVACGlobals::TimeStepSys;
        using DataHVACGlobals::WaterCooled;
        using DXCoils::DXCoil;
        using DXCoils::NumDXCoils;
        using EvaporativeCoolers::EvapCond;
        using EvaporativeCoolers::NumEvapCool;
        using EvaporativeFluidCoolers::NumSimpleEvapFluidCoolers;
        using EvaporativeFluidCoolers::SimpleEvapFluidCooler;
        using FluidCoolers::SimpleFluidCooler;
        using HeatingCoils::HeatingCoil;
        using HeatingCoils::NumHeatingCoils;
        using HVACVariableRefrigerantFlow::NumVRFCond;
        using HVACVariableRefrigerantFlow::VRF;
        using MixedAir::NumOAControllers;
        using MixedAir::OAController;
        using PackagedThermalStorageCoil::NumTESCoils;
        using PackagedThermalStorageCoil::TESCoil;

        static int iOACtrl(0);
        static int iCoil(0);
        static int iCooler(0);
        static int iChiller(0);
        static int iBoiler(0);
        static int iTank(0);
        static int iRef(0);

        static Real64 H2OHtOfVap_HVAC = Psychrometrics::PsyHgAirFnWTdb(state.dataEnvrn->OutHumRat, state.dataEnvrn->OutDryBulbTemp);
        static Real64 RhoWater = Psychrometrics::RhoH2O(state.dataEnvrn->OutDryBulbTemp);
        Real64 TimeStepSysSec = TimeStepSys * DataGlobalConstants::SecInHour;
        SysTotalHVACReliefHeatLoss = 0;
        SysTotalHVACRejectHeatLoss = 0;

        // HVAC relief air
        for (iOACtrl = 1; iOACtrl <= NumOAControllers; ++iOACtrl) {
            SysTotalHVACReliefHeatLoss += OAController(iOACtrl).RelTotalLossRate * TimeStepSysSec;
        }

        // Condenser water loop
        for (iCooler = 1; iCooler <= state.dataCondenserLoopTowers->NumSimpleTowers; ++iCooler) {
            SysTotalHVACRejectHeatLoss +=
                state.dataCondenserLoopTowers->towers(iCooler).Qactual * TimeStepSysSec + state.dataCondenserLoopTowers->towers(iCooler).FanEnergy + state.dataCondenserLoopTowers->towers(iCooler).BasinHeaterConsumption;
        }
        for (iCooler = 1; iCooler <= NumSimpleEvapFluidCoolers; ++iCooler) {
            SysTotalHVACRejectHeatLoss += SimpleEvapFluidCooler(iCooler).Qactual * TimeStepSysSec + SimpleEvapFluidCooler(iCooler).FanEnergy;
        }
        for (auto &cooler : SimpleFluidCooler) {
            SysTotalHVACRejectHeatLoss += cooler.Qactual * TimeStepSysSec + cooler.FanEnergy;
        }

        // Air- and Evap-cooled chiller
        for (iChiller = 1; iChiller <= state.dataPlantChillers->NumElectricChillers; ++iChiller) {
            if (state.dataPlantChillers->ElectricChiller(iChiller).CondenserType != DataPlant::CondenserType::WATERCOOLED) {
                SysTotalHVACRejectHeatLoss += state.dataPlantChillers->ElectricChiller(iChiller).CondenserEnergy;
            }
        }
        for (iChiller = 1; iChiller <= state.dataPlantChillers->NumEngineDrivenChillers; ++iChiller) {
            if (state.dataPlantChillers->EngineDrivenChiller(iChiller).CondenserType != DataPlant::CondenserType::WATERCOOLED) {
                SysTotalHVACRejectHeatLoss += state.dataPlantChillers->EngineDrivenChiller(iChiller).CondenserEnergy;
            }
        }
        for (iChiller = 1; iChiller <= state.dataPlantChillers->NumGTChillers; ++iChiller) {
            if (state.dataPlantChillers->GTChiller(iChiller).CondenserType != DataPlant::CondenserType::WATERCOOLED) {
                SysTotalHVACRejectHeatLoss += state.dataPlantChillers->GTChiller(iChiller).CondenserEnergy;
            }
        }
        for (iChiller = 1; iChiller <= state.dataPlantChillers->NumConstCOPChillers; ++iChiller) {
            if (state.dataPlantChillers->ConstCOPChiller(iChiller).CondenserType != DataPlant::CondenserType::WATERCOOLED) {
                SysTotalHVACRejectHeatLoss += state.dataPlantChillers->ConstCOPChiller(iChiller).CondenserEnergy;
            }
        }
        for (iChiller = 1; iChiller <= state.dataChillerElectricEIR->NumElectricEIRChillers; ++iChiller) {
            if (state.dataChillerElectricEIR->ElectricEIRChiller(iChiller).CondenserType != DataPlant::CondenserType::WATERCOOLED) {
                SysTotalHVACRejectHeatLoss += state.dataChillerElectricEIR->ElectricEIRChiller(iChiller).CondEnergy;
            }
        }
        for (iChiller = 1; iChiller <= state.dataChillerReformulatedEIR->NumElecReformEIRChillers; ++iChiller) {
            if (state.dataChillerReformulatedEIR->ElecReformEIRChiller(iChiller).CondenserType != DataPlant::CondenserType::WATERCOOLED) {
                SysTotalHVACRejectHeatLoss += state.dataChillerReformulatedEIR->ElecReformEIRChiller(iChiller).CondEnergy;
            }
        }

        // Water / steam boiler
        for (iBoiler = 1; iBoiler <= state.dataBoilers->numBoilers; ++iBoiler) {
            SysTotalHVACRejectHeatLoss += state.dataBoilers->Boiler(iBoiler).FuelConsumed + state.dataBoilers->Boiler(iBoiler).ParasiticElecConsumption - state.dataBoilers->Boiler(iBoiler).BoilerEnergy;
        }

        // DX Coils air to air
        for (iCoil = 1; iCoil <= NumDXCoils; ++iCoil) {
            if (DXCoil(iCoil).DXCoilType_Num == DataHVACGlobals::CoilDX_CoolingSingleSpeed ||
                DXCoil(iCoil).DXCoilType_Num == DataHVACGlobals::CoilDX_CoolingTwoSpeed ||
                DXCoil(iCoil).DXCoilType_Num == DataHVACGlobals::CoilDX_MultiSpeedCooling ||
                DXCoil(iCoil).DXCoilType_Num == DataHVACGlobals::CoilDX_CoolingTwoStageWHumControl) {
                if (DXCoil(iCoil).CondenserType(1) == AirCooled) {
                    SysTotalHVACRejectHeatLoss += DXCoil(iCoil).ElecCoolingConsumption + DXCoil(iCoil).DefrostConsumption +
                                                  DXCoil(iCoil).CrankcaseHeaterConsumption + DXCoil(iCoil).TotalCoolingEnergy;
                } else if (DXCoil(iCoil).CondenserType(1) == EvapCooled) {
                    SysTotalHVACRejectHeatLoss += DXCoil(iCoil).EvapCondPumpElecConsumption + DXCoil(iCoil).BasinHeaterConsumption +
                                                  DXCoil(iCoil).EvapWaterConsump * RhoWater * H2OHtOfVap_HVAC;
                }
                if (DXCoil(iCoil).FuelTypeNum != DataGlobalConstants::ResourceType::Electricity) {
                    SysTotalHVACRejectHeatLoss += DXCoil(iCoil).MSFuelWasteHeat * TimeStepSysSec;
                }
            } else if (DXCoil(iCoil).DXCoilType_Num == DataHVACGlobals::CoilDX_HeatingEmpirical ||
                       DXCoil(iCoil).DXCoilType_Num == DataHVACGlobals::CoilDX_MultiSpeedHeating) {
                SysTotalHVACRejectHeatLoss += DXCoil(iCoil).ElecHeatingConsumption + DXCoil(iCoil).DefrostConsumption + DXCoil(iCoil).FuelConsumed +
                                              DXCoil(iCoil).CrankcaseHeaterConsumption - DXCoil(iCoil).TotalHeatingEnergy;
            }
        }
        // VAV coils - air to air
        for (iCoil = 1; iCoil <= state.dataVariableSpeedCoils->NumVarSpeedCoils; ++iCoil) {
            if (state.dataVariableSpeedCoils->VarSpeedCoil(iCoil).VSCoilTypeOfNum == DataHVACGlobals::Coil_CoolingAirToAirVariableSpeed) {
                if (state.dataVariableSpeedCoils->VarSpeedCoil(iCoil).CondenserType == AirCooled) {
                    SysTotalHVACRejectHeatLoss += state.dataVariableSpeedCoils->VarSpeedCoil(iCoil).Energy + state.dataVariableSpeedCoils->VarSpeedCoil(iCoil).CrankcaseHeaterConsumption +
                                                  state.dataVariableSpeedCoils->VarSpeedCoil(iCoil).DefrostConsumption + state.dataVariableSpeedCoils->VarSpeedCoil(iCoil).EnergyLoadTotal;
                } else if (state.dataVariableSpeedCoils->VarSpeedCoil(iCoil).CondenserType == EvapCooled) {
                    SysTotalHVACRejectHeatLoss += state.dataVariableSpeedCoils->VarSpeedCoil(iCoil).EvapCondPumpElecConsumption + state.dataVariableSpeedCoils->VarSpeedCoil(iCoil).BasinHeaterConsumption +
                                                  state.dataVariableSpeedCoils->VarSpeedCoil(iCoil).EvapWaterConsump * RhoWater * H2OHtOfVap_HVAC;
                }
            } else if (state.dataVariableSpeedCoils->VarSpeedCoil(iCoil).VSCoilTypeOfNum == DataHVACGlobals::Coil_HeatingAirToAirVariableSpeed) {
                SysTotalHVACRejectHeatLoss += state.dataVariableSpeedCoils->VarSpeedCoil(iCoil).Energy + state.dataVariableSpeedCoils->VarSpeedCoil(iCoil).CrankcaseHeaterConsumption +
                                              state.dataVariableSpeedCoils->VarSpeedCoil(iCoil).DefrostConsumption - state.dataVariableSpeedCoils->VarSpeedCoil(iCoil).EnergyLoadTotal;
            }
        }

        // Heating coils - fuel
        for (iCoil = 1; iCoil <= NumHeatingCoils; ++iCoil) {
            if (HeatingCoil(iCoil).HCoilType_Num == DataHVACGlobals::Coil_HeatingGas_MultiStage ||
                HeatingCoil(iCoil).HCoilType_Num == DataHVACGlobals::Coil_HeatingGasOrOtherFuel) {
                SysTotalHVACRejectHeatLoss +=
                    HeatingCoil(iCoil).FuelUseLoad + HeatingCoil(iCoil).ParasiticFuelLoad - HeatingCoil(iCoil).HeatingCoilLoad;
            }
        }

        // Packaged TES
        for (iCoil = 1; iCoil <= NumTESCoils; ++iCoil) {
            if (TESCoil(iCoil).CondenserType == AirCooled) {
                SysTotalHVACRejectHeatLoss += TESCoil(iCoil).EvapTotCoolingEnergy + TESCoil(iCoil).ElecCoolingEnergy +
                                              TESCoil(iCoil).ElectColdWeatherEnergy - TESCoil(iCoil).Q_Ambient;
            } else if (TESCoil(iCoil).CondenserType == EvapCooled) {
                SysTotalHVACRejectHeatLoss += TESCoil(iCoil).EvapCondPumpElecConsumption + TESCoil(iCoil).ElectEvapCondBasinHeaterEnergy +
                                              TESCoil(iCoil).EvapWaterConsump * RhoWater * H2OHtOfVap_HVAC - TESCoil(iCoil).Q_Ambient;
            }
        }

        // Water heater and thermal storage
        for (iTank = 1; iTank <= state.dataWaterThermalTanks->numWaterThermalTank; ++iTank) {
            if (state.dataWaterThermalTanks->WaterThermalTank(iTank).AmbientTempIndicator == WaterThermalTanks::AmbientTempEnum::OutsideAir) {
                SysTotalHVACRejectHeatLoss += state.dataWaterThermalTanks->WaterThermalTank(iTank).FuelEnergy - state.dataWaterThermalTanks->WaterThermalTank(iTank).TotalDemandEnergy;
            }
        }

        // Variable Refrigerant Flow
        for (iCoil = 1; iCoil <= NumVRFCond; ++iCoil) {
            if (VRF(iCoil).CondenserType == AirCooled) {
                SysTotalHVACRejectHeatLoss += VRF(iCoil).CoolElecConsumption + VRF(iCoil).HeatElecConsumption +
                                              VRF(iCoil).CrankCaseHeaterElecConsumption + VRF(iCoil).DefrostConsumption +
                                              (VRF(iCoil).TotalCoolingCapacity - VRF(iCoil).TotalHeatingCapacity) * TimeStepSysSec;
            } else if (VRF(iCoil).CondenserType == EvapCooled) {
                SysTotalHVACRejectHeatLoss += VRF(iCoil).EvapCondPumpElecConsumption + VRF(iCoil).BasinHeaterConsumption +
                                              VRF(iCoil).EvapWaterConsumpRate * TimeStepSysSec * RhoWater * H2OHtOfVap_HVAC;
            } else if (VRF(iCoil).CondenserType == WaterCooled) {
                SysTotalHVACRejectHeatLoss += VRF(iCoil).QCondEnergy;
            }
        }

        // Refrigerated Rack
        auto &RefrigRack(state.dataRefrigCase->RefrigRack);
        for (iRef = 1; iRef <= NumRefrigeratedRacks; ++iRef) {
            if (RefrigRack(iRef).CondenserType == AirCooled) {
                SysTotalHVACRejectHeatLoss += RefrigRack(iRef).RackElecConsumption + RefrigRack(iRef).RackCoolingEnergy;
            } else if (RefrigRack(iRef).CondenserType == EvapCooled) {
                SysTotalHVACRejectHeatLoss += RefrigRack(iRef).EvapPumpConsumption + RefrigRack(iRef).BasinHeaterConsumption +
                                              RefrigRack(iRef).EvapWaterConsumption * RhoWater * H2OHtOfVap_HVAC;
            } else if (RefrigRack(iRef).CondenserType == WaterCooled) {
                SysTotalHVACRejectHeatLoss += RefrigRack(iRef).CondEnergy;
            }
        }

        // Refrigerated Case - Condenser
        for (iRef = 1; iRef <= NumRefrigCondensers; ++iRef) {
            SysTotalHVACRejectHeatLoss += state.dataRefrigCase->Condenser(iRef).CondEnergy;
        }

        // Evaporative coolers
        for (iCooler = 1; iCooler <= NumEvapCool; ++iCooler) {
            SysTotalHVACRejectHeatLoss += EvapCond(iCooler).EvapWaterConsump * RhoWater * H2OHtOfVap_HVAC + EvapCond(iCooler).EvapCoolerEnergy;
        }
    }

    void GatherHeatGainReport(EnergyPlusData &state, OutputProcessor::TimeStepType t_timeStepType) // What kind of data to update (Zone, HVAC)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   August 2011
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Gathers the data each zone timestep for the heat gain report.
        // The routine generates an annual table with the following columns which correspond to
        // the output variables and data structures shown:
        // Column                               Output Variable                                Internal Data Structure      Timestep Type
        // ------                               ---------------                                -----------------------      -------- -----
        // HVAC Input Sensible Air Heating      Zone Air Heat Balance System Air Transfer Rate ZnAirRpt()%SumMCpDTsystem    HVAC     Rate
        //                                   Zone Air Heat Balance System Convective Heat Gain Rate ZnAirRpt()%SumNonAirSystem HVAC   Rate
        // HVAC Input Sensible Air Cooling      Zone Air Heat Balance System Air Transfer Rate ZnAirRpt()%SumMCpDTsystem    HVAC     Rate
        // HVAC sensible heating by ATU         sensible heating by the air terminal unit                                   HVAC     Rate
        // HVAC sensible cooling by ATU         sensible cooling by the air terminal unit                                   HVAC     Rate
        //                                    Zone Air Heat Balance System Convective Heat Gain Rate ZnAirRpt()%SumNonAirSystem HVAC  Rate
        // HVAC Input Heated Surface Heating    Electric Low Temp Radiant Heating Energy       ElecRadSys()%HeatEnergy      HVAC     Energy
        //                                      Zone Ventilated Slab Radiant Heating Energy    VentSlab()%RadHeatingEnergy  HVAC     Energy
        //                                      Hydronic Low Temp Radiant Heating Energy       HydrRadSys()%HeatEnergy      HVAC     Energy
        //                                      Constant Flow Low Temp Radiant Heating Energy  CFloRadSys()%HeatEnergy      HVAC     Energy
        // HVAC Input Cooled Surface Cooling    Zone Ventilated Slab Radiant Cooling Energy    -VentSlab()%RadCoolingEnergy HVAC     Energy
        //                                      Hydronic Low Temp Radiant Cooling Energy       -HydrRadSys()%CoolEnergy     HVAC     Energy
        //                                      Constant Flow Low Temp Radiant Cooling Energy  -CFloRadSys()%CoolEnergy     HVAC     Energy
        // People Sensible Heat Addition        Zone People Sensible Heating Energy            ZnRpt()%PeopleSenGain        Zone     Energy
        // Lights Sensible Heat Addition        Zone Lights Total Heating Energy               ZnRpt()%LtsTotGain           Zone     Energy
        // Equipment Sensible Heat Addition     Zone Electric Equipment Radiant Heating Energy ZnRpt()%ElecRadGain          Zone     Energy
        //                                      Zone Gas Equipment Radiant Heating Energy      ZnRpt()%GasRadGain           Zone     Energy
        //                                      Zone Steam Equipment Radiant Heating Energy    ZnRpt()%SteamRadGain         Zone     Energy
        //                                      Zone Hot Water Equipment Radiant Heating Energy ZnRpt()%HWRadGain           Zone     Energy
        //                                      Zone Other Equipment Radiant Heating Energy    ZnRpt()%OtherRadGain         Zone     Energy
        //                                   Zone Electric Equipment Convective Heating Energy ZnRpt()%ElecConGain          Zone     Energy
        //                                      Zone Gas Equipment Convective Heating Energy   ZnRpt()%GasConGain           Zone     Energy
        //                                      Zone Steam Equipment Convective Heating Energy ZnRpt()%SteamConGain         Zone     Energy
        //                                    Zone Hot Water Equipment Convective Heating Energy ZnRpt()%HWConGain          Zone     Energy
        //                                      Zone Other Equipment Convective Heating Energy ZnRpt()%OtherConGain         Zone     Energy
        // Window Heat Addition                 Zone Windows Total Heat Gain Energy            ZoneWinHeatGainRepEnergy()   Zone     Energy
        // Interzone Air Transfer Heat Addition Zone Air Heat Balance Interzone Air Transfer Rate  ZnAirRpt()%SumMCpDTzones HVAC     Rate
        // Infiltration Heat Addition           Zone Air Heat Balance Outdoor Air Transfer Rate ZnAirRpt()%SumMCpDtInfil     HVAC     Rate
        // Equipment Sensible Heat Removal      Zone Electric Equipment Radiant Heating Energy ZnRpt()%ElecRadGain          Zone     Energy
        //                                      Zone Gas Equipment Radiant Heating Energy      ZnRpt()%GasRadGain           Zone     Energy
        //                                      Zone Steam Equipment Radiant Heating Energy    ZnRpt()%SteamRadGain         Zone     Energy
        //                                      Zone Hot Water Equipment Radiant Heating Energy ZnRpt()%HWRadGain           Zone     Energy
        //                                      Zone Other Equipment Radiant Heating Energy    ZnRpt()%OtherRadGain         Zone     Energy
        //                                   Zone Electric Equipment Convective Heating Energy ZnRpt()%ElecConGain          Zone     Energy
        //                                      Zone Gas Equipment Convective Heating Energy   ZnRpt()%GasConGain           Zone     Energy
        //                                      Zone Steam Equipment Convective Heating Energy ZnRpt()%SteamConGain         Zone     Energy
        //                                     Zone Hot Water Equipment Convective Heating Energy ZnRpt()%HWConGain         Zone     Energy
        //                                      Zone Other Equipment Convective Heating Energy ZnRpt()%OtherConGain         Zone     Energy
        // Window Heat Removal                  Zone Windows Total Heat Loss Energy            -ZoneWinHeatLossRepEnergy()  Zone     Energy
        // Interzone Air Transfer Heat Removal  Zone Air Heat Balance Interzone Air Transfer Rate ZnAirRpt()%SumMCpDTzones  HVAC     Rate
        // Infiltration Heat Removal            Zone Air Heat Balance Outdoor Air Transfer Rate ZnAirRpt()%SumMCpDtInfil     HVAC     Rate
        // The following two columns are derived based on the values of the other columns and need to be computed on every HVAC timestep.
        //   Opaque Surface Conduction and Other Heat Addition
        //   Opaque Surface Conduction and Other Heat Removal
        // For variables that are updated on a zone timestep basis, the values are used on the HVAC timestep but are ratioed by the
        // timestep lengths.
        // The peak reports follow a similar example.

        // Using/Aliasing
        using DataHeatBalance::BuildingPreDefRep;
        using DataHeatBalance::ZnAirRpt;
        using DataHeatBalance::ZnRpt;
        using DataHeatBalance::Zone;
        using DataHeatBalance::ZonePreDefRep;
        using DataHeatBalance::ZoneWinHeatGainRep;
        using DataHeatBalance::ZoneWinHeatGainRepEnergy;
        using DataHeatBalance::ZoneWinHeatLossRep;
        using DataHeatBalance::ZoneWinHeatLossRepEnergy;
        using DataHVACGlobals::TimeStepSys;
        using General::DetermineMinuteForReporting;
        using General::EncodeMonDayHrMin;
        using LowTempRadiantSystem::CFloRadSys;
        using LowTempRadiantSystem::ElecRadSys;
        using LowTempRadiantSystem::HydrRadSys;
        using LowTempRadiantSystem::NumOfCFloLowTempRadSys;
        using LowTempRadiantSystem::NumOfElecLowTempRadSys;
        using LowTempRadiantSystem::NumOfHydrLowTempRadSys;

        static int iZone(0);
        static int iRadiant(0);
        static int iunit(0);
        static int curZone(0);
        static Real64 eqpSens(0.0);
        static Real64 total(0.0);
        // the following arrays store the radiant total for each timestep
        static Array1D<Real64> radiantHeat;
        static Array1D<Real64> radiantCool;
        static Array1D<Real64> ATUHeat;
        static Array1D<Real64> ATUCool;
        static int timestepTimeStamp(0);
        static Real64 bldgHtPk(0.0);
        static Real64 bldgClPk(0.0);
        static Real64 timeStepRatio(0.0);

        Real64 mult; // zone list and group multipliers

        int ActualTimeMin;

        if (!state.dataGlobal->DoWeathSim) return;

        if (!state.dataOutRptPredefined->reportName(state.dataOutRptPredefined->pdrSensibleGain).show) return; // don't gather data if report isn't requested

        if (t_timeStepType == OutputProcessor::TimeStepType::TimeStepZone) return; // only add values over the HVAC timestep basis

        auto &ort(state.dataOutRptTab);

        if (ort->GatherHeatGainReportfirstTime) {
            radiantHeat.allocate(state.dataGlobal->NumOfZones);
            radiantCool.allocate(state.dataGlobal->NumOfZones);
            ATUHeat.allocate(state.dataGlobal->NumOfZones);
            ATUCool.allocate(state.dataGlobal->NumOfZones);
            ort->GatherHeatGainReportfirstTime = false;
        }
        // clear the radiant surface accumulation variables
        radiantHeat = 0.0;
        radiantCool = 0.0;
        // clear the ATU accumulation variables
        ATUHeat = 0.0;
        ATUCool = 0.0;
        //--------------------
        //     ANNUAL
        //--------------------
        // HVAC annual heating by ATU
        // HVAC annual cooling by ATU
        for (iunit = 1; iunit <= state.dataDefineEquipment->NumAirDistUnits; ++iunit) {
            // HVAC equipment should already have the multipliers included, no "* mult" needed (assumes autosized or multiplied hard-sized air flow).
            curZone = state.dataDefineEquipment->AirDistUnit(iunit).ZoneNum;
            if ((curZone > 0) && (curZone <= state.dataGlobal->NumOfZones)) {
                ZonePreDefRep(curZone).SHGSAnHvacATUHt += state.dataDefineEquipment->AirDistUnit(iunit).HeatGain;
                ZonePreDefRep(curZone).SHGSAnHvacATUCl -= state.dataDefineEquipment->AirDistUnit(iunit).CoolGain;
                ATUHeat(curZone) += state.dataDefineEquipment->AirDistUnit(iunit).HeatRate;
                ATUCool(curZone) -= state.dataDefineEquipment->AirDistUnit(iunit).CoolRate;
            }
        }
        timeStepRatio = TimeStepSys / state.dataGlobal->TimeStepZone; // the fraction of the zone time step used by the system timestep
        for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
            mult = Zone(iZone).Multiplier * Zone(iZone).ListMultiplier;
            // People Sensible Heat Addition
            ZonePreDefRep(iZone).SHGSAnPeoplAdd += ZnRpt(iZone).PeopleSenGain * mult * timeStepRatio;
            // Lights Sensible Heat Addition
            ZonePreDefRep(iZone).SHGSAnLiteAdd += ZnRpt(iZone).LtsTotGain * mult * timeStepRatio;
            // HVAC Input Sensible Air Heating
            // HVAC Input Sensible Air Cooling
            Real64 ZoneEqHeatorCool =
                ZnAirRpt(iZone).SumMCpDTsystem + ZnAirRpt(iZone).SumNonAirSystem * mult - ATUHeat(iZone) - ATUCool(iZone);
            if (ZoneEqHeatorCool > 0.0) {
                ZonePreDefRep(iZone).SHGSAnZoneEqHt += ZoneEqHeatorCool * TimeStepSys * DataGlobalConstants::SecInHour;
            } else {
                ZonePreDefRep(iZone).SHGSAnZoneEqCl += ZoneEqHeatorCool * TimeStepSys * DataGlobalConstants::SecInHour;
            }
            // Interzone Air Transfer Heat Addition
            // Interzone Air Transfer Heat Removal
            if (ZnAirRpt(iZone).SumMCpDTzones > 0.0) {
                ZonePreDefRep(iZone).SHGSAnIzaAdd += ZnAirRpt(iZone).SumMCpDTzones * mult * TimeStepSys * DataGlobalConstants::SecInHour;
            } else {
                ZonePreDefRep(iZone).SHGSAnIzaRem += ZnAirRpt(iZone).SumMCpDTzones * mult * TimeStepSys * DataGlobalConstants::SecInHour;
            }
            // Window Heat Addition
            // Window Heat Removal
            ZonePreDefRep(iZone).SHGSAnWindAdd += ZoneWinHeatGainRepEnergy(iZone) * mult * timeStepRatio;
            ZonePreDefRep(iZone).SHGSAnWindRem -= ZoneWinHeatLossRepEnergy(iZone) * mult * timeStepRatio;
            // Infiltration Heat Addition
            // Infiltration Heat Removal
            if (ZnAirRpt(iZone).SumMCpDtInfil > 0.0) {
                ZonePreDefRep(iZone).SHGSAnInfilAdd += ZnAirRpt(iZone).SumMCpDtInfil * mult * TimeStepSys * DataGlobalConstants::SecInHour;
            } else {
                ZonePreDefRep(iZone).SHGSAnInfilRem += ZnAirRpt(iZone).SumMCpDtInfil * mult * TimeStepSys * DataGlobalConstants::SecInHour;
            }
            // Equipment Sensible Heat Addition
            // Equipment Sensible Heat Removal
            // the following variables are already gains so they do not need to be converted by multiplying by time.
            eqpSens = (ZnRpt(iZone).ElecRadGain + ZnRpt(iZone).GasRadGain + ZnRpt(iZone).HWRadGain + ZnRpt(iZone).SteamRadGain +
                       ZnRpt(iZone).OtherRadGain + ZnRpt(iZone).ElecConGain + ZnRpt(iZone).GasConGain + ZnRpt(iZone).HWConGain +
                       ZnRpt(iZone).SteamConGain + ZnRpt(iZone).OtherConGain) *
                      timeStepRatio;
            if (eqpSens > 0.0) {
                ZonePreDefRep(iZone).SHGSAnEquipAdd += eqpSens * mult;
            } else {
                ZonePreDefRep(iZone).SHGSAnEquipRem += eqpSens * mult;
            }
        }
        curZone = 0;
        // HVAC Input Heated Surface Heating
        // HVAC Input Cooled Surface Cooling
        for (iRadiant = 1; iRadiant <= state.dataVentilatedSlab->NumOfVentSlabs; ++iRadiant) {
            curZone = state.dataVentilatedSlab->VentSlab(iRadiant).ZonePtr;
            mult = Zone(curZone).Multiplier * Zone(curZone).ListMultiplier;
            if ((curZone > 0) && (curZone <= state.dataGlobal->NumOfZones)) {
                ZonePreDefRep(curZone).SHGSAnSurfHt += state.dataVentilatedSlab->VentSlab(iRadiant).RadHeatingEnergy * mult;
                ZonePreDefRep(curZone).SHGSAnSurfCl -= state.dataVentilatedSlab->VentSlab(iRadiant).RadCoolingEnergy * mult;
                radiantHeat(curZone) = state.dataVentilatedSlab->VentSlab(iRadiant).RadHeatingPower * mult;
                radiantCool(curZone) = -state.dataVentilatedSlab->VentSlab(iRadiant).RadCoolingPower * mult;
            }
        }
        for (iRadiant = 1; iRadiant <= NumOfHydrLowTempRadSys; ++iRadiant) {
            curZone = HydrRadSys(iRadiant).ZonePtr;
            mult = Zone(curZone).Multiplier * Zone(curZone).ListMultiplier;
            if ((curZone > 0) && (curZone <= state.dataGlobal->NumOfZones)) {
                ZonePreDefRep(curZone).SHGSAnSurfHt += HydrRadSys(iRadiant).HeatEnergy * mult;
                ZonePreDefRep(curZone).SHGSAnSurfCl -= HydrRadSys(iRadiant).CoolEnergy * mult;
                radiantHeat(curZone) += HydrRadSys(iRadiant).HeatPower * mult;
                radiantCool(curZone) -= HydrRadSys(iRadiant).CoolPower * mult;
            }
        }
        for (iRadiant = 1; iRadiant <= NumOfCFloLowTempRadSys; ++iRadiant) {
            curZone = CFloRadSys(iRadiant).ZonePtr;
            mult = Zone(curZone).Multiplier * Zone(curZone).ListMultiplier;
            if ((curZone > 0) && (curZone <= state.dataGlobal->NumOfZones)) {
                ZonePreDefRep(curZone).SHGSAnSurfHt += CFloRadSys(iRadiant).HeatEnergy * mult;
                ZonePreDefRep(curZone).SHGSAnSurfCl -= CFloRadSys(iRadiant).CoolEnergy * mult;
                radiantHeat(curZone) += CFloRadSys(iRadiant).HeatPower * mult;
                radiantCool(curZone) -= CFloRadSys(iRadiant).CoolPower * mult;
            }
        }
        for (iRadiant = 1; iRadiant <= NumOfElecLowTempRadSys; ++iRadiant) {
            curZone = ElecRadSys(iRadiant).ZonePtr;
            mult = Zone(curZone).Multiplier * Zone(curZone).ListMultiplier;
            if ((curZone > 0) && (curZone <= state.dataGlobal->NumOfZones)) {
                ZonePreDefRep(curZone).SHGSAnSurfHt += ElecRadSys(iRadiant).HeatEnergy * mult;
                radiantHeat(curZone) += ElecRadSys(iRadiant).HeatPower * mult;
            }
        }
        // Opaque Surface Conduction and Other Heat Addition
        // Opaque Surface Conduction and Other Heat Removal
        for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
            // ZonePreDefRep variables above already inlude zone list and group multipliers
            total = ZonePreDefRep(iZone).SHGSAnPeoplAdd + ZonePreDefRep(iZone).SHGSAnLiteAdd + ZonePreDefRep(iZone).SHGSAnZoneEqHt +
                    ZonePreDefRep(iZone).SHGSAnZoneEqCl + ZonePreDefRep(iZone).SHGSAnHvacATUHt + ZonePreDefRep(iZone).SHGSAnHvacATUCl +
                    ZonePreDefRep(iZone).SHGSAnIzaAdd + ZonePreDefRep(iZone).SHGSAnIzaRem +
                    ZonePreDefRep(iZone).SHGSAnWindAdd + ZonePreDefRep(iZone).SHGSAnWindRem + ZonePreDefRep(iZone).SHGSAnInfilAdd +
                    ZonePreDefRep(iZone).SHGSAnInfilRem + ZonePreDefRep(iZone).SHGSAnEquipAdd + ZonePreDefRep(iZone).SHGSAnEquipRem +
                    ZonePreDefRep(iZone).SHGSAnSurfHt + ZonePreDefRep(iZone).SHGSAnSurfCl;
            total = -total; // want to know the negative value of the sum since the row should add up to zero
            if (total > 0) {
                ZonePreDefRep(iZone).SHGSAnOtherAdd = total;
            } else {
                ZonePreDefRep(iZone).SHGSAnOtherRem = total;
            }
        }
        //--------------------------------
        // ZONE PEAK COOLING AND HEATING
        //--------------------------------
        for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
            mult = Zone(iZone).Multiplier * Zone(iZone).ListMultiplier;
            // RR I can't get the Infiltration Heat Addition/Removal columns to add up.
            // THis is the only suspect line, mixing MCpDt terms and a power term looks fishy.
            if ((ZnAirRpt(iZone).SumMCpDTsystem + radiantHeat(iZone) + ZnAirRpt(iZone).SumNonAirSystem * mult) > 0) {
                if ((ZnAirRpt(iZone).SumMCpDTsystem + radiantHeat(iZone) + ZnAirRpt(iZone).SumNonAirSystem * mult) > ZonePreDefRep(iZone).htPeak) {
                    ZonePreDefRep(iZone).htPeak = ZnAirRpt(iZone).SumMCpDTsystem + radiantHeat(iZone) + ZnAirRpt(iZone).SumNonAirSystem * mult;
                    // determine timestamp
                    //      ActualTimeS = CurrentTime-TimeStepZone+SysTimeElapsed
                    //      ActualtimeE = ActualTimeS+TimeStepSys
                    //      ActualTimeHrS=INT(ActualTimeS)
                    //      ActualTimeMin=NINT((ActualtimeE - ActualTimeHrS)*FracToMin)
                    ActualTimeMin = DetermineMinuteForReporting(state, t_timeStepType);
                    EncodeMonDayHrMin(timestepTimeStamp, state.dataEnvrn->Month, state.dataEnvrn->DayOfMonth, state.dataGlobal->HourOfDay, ActualTimeMin);
                    ZonePreDefRep(iZone).htPtTimeStamp = timestepTimeStamp;
                    // HVAC Input Sensible Air Heating
                    // HVAC Input Sensible Air Cooling
                    // non-HVAC ZnAirRpt variables DO NOT include zone multipliers
                    ZonePreDefRep(iZone).SHGSHtHvacHt = ZnAirRpt(iZone).SumMCpDTsystem + ZnAirRpt(iZone).SumNonAirSystem * mult;
                    ZonePreDefRep(iZone).SHGSHtHvacCl = 0.0;
                    // HVAC Input Heated Surface Heating
                    // HVAC Input Cooled Surface Cooling
                    ZonePreDefRep(iZone).SHGSHtSurfHt = radiantHeat(iZone); // multipliers included above
                    ZonePreDefRep(iZone).SHGSHtSurfCl = radiantCool(iZone); // multipliers included above
                    // HVAC ATU Heating at Heat Peak
                    // HVAC ATU Cooling at Heat Peak
                    ZonePreDefRep(iZone).SHGSHtHvacATUHt = ATUHeat(iZone); // multipliers included above
                    ZonePreDefRep(iZone).SHGSHtHvacATUCl = ATUCool(iZone); // multipliers included above
                    // People Sensible Heat Addition
                    ZonePreDefRep(iZone).SHGSHtPeoplAdd = ZnRpt(iZone).PeopleSenGainRate * mult;
                    // Lights Sensible Heat Addition
                    ZonePreDefRep(iZone).SHGSHtLiteAdd = ZnRpt(iZone).LtsTotGainRate * mult;
                    // Equipment Sensible Heat Addition
                    // Equipment Sensible Heat Removal
                    // non-HVAC ZnAirRpt variables DO NOT include zone multipliers
                    eqpSens = ZnRpt(iZone).ElecRadGainRate + ZnRpt(iZone).GasRadGainRate + ZnRpt(iZone).HWRadGainRate +
                              ZnRpt(iZone).SteamRadGainRate + ZnRpt(iZone).OtherRadGainRate + ZnRpt(iZone).ElecConGainRate +
                              ZnRpt(iZone).GasConGainRate + ZnRpt(iZone).HWConGainRate + ZnRpt(iZone).SteamConGainRate +
                              ZnRpt(iZone).OtherConGainRate;
                    if (eqpSens > 0.0) {
                        ZonePreDefRep(iZone).SHGSHtEquipAdd = eqpSens * mult;
                        ZonePreDefRep(iZone).SHGSHtEquipRem = 0.0;
                    } else {
                        ZonePreDefRep(iZone).SHGSHtEquipAdd = 0.0;
                        ZonePreDefRep(iZone).SHGSHtEquipRem = eqpSens * mult;
                    }
                    // Window Heat Addition
                    // Window Heat Removal
                    ZonePreDefRep(iZone).SHGSHtWindAdd = ZoneWinHeatGainRep(iZone) * mult;
                    ZonePreDefRep(iZone).SHGSHtWindRem = -ZoneWinHeatLossRep(iZone) * mult;
                    // mixing object heat addition and removal
                    if (ZnAirRpt(iZone).SumMCpDTzones > 0.0) {
                        ZonePreDefRep(iZone).SHGSHtIzaAdd = ZnAirRpt(iZone).SumMCpDTzones * mult;
                        ZonePreDefRep(iZone).SHGSHtIzaRem = 0.0;
                    } else {
                        ZonePreDefRep(iZone).SHGSHtIzaAdd = 0.0;
                        ZonePreDefRep(iZone).SHGSHtIzaRem = ZnAirRpt(iZone).SumMCpDTzones * mult;
                    }
                    // Infiltration Heat Addition
                    // Infiltration Heat Removal
                    if (ZnAirRpt(iZone).SumMCpDtInfil > 0.0) {
                        ZonePreDefRep(iZone).SHGSHtInfilAdd = ZnAirRpt(iZone).SumMCpDtInfil * mult;
                        ZonePreDefRep(iZone).SHGSHtInfilRem = 0.0;
                    } else {
                        ZonePreDefRep(iZone).SHGSHtInfilAdd = 0.0;
                        ZonePreDefRep(iZone).SHGSHtInfilRem = ZnAirRpt(iZone).SumMCpDtInfil * mult;
                    }
                    // Opaque Surface Conduction and Other Heat Addition
                    // Opaque Surface Conduction and Other Heat Removal
                    total = ZonePreDefRep(iZone).SHGSHtPeoplAdd + ZonePreDefRep(iZone).SHGSHtLiteAdd + ZonePreDefRep(iZone).SHGSHtHvacHt +
                            ZonePreDefRep(iZone).SHGSHtHvacCl + ZonePreDefRep(iZone).SHGSHtIzaAdd + ZonePreDefRep(iZone).SHGSHtIzaRem +
                            ZonePreDefRep(iZone).SHGSHtWindAdd + ZonePreDefRep(iZone).SHGSHtWindRem + ZonePreDefRep(iZone).SHGSHtInfilAdd +
                            ZonePreDefRep(iZone).SHGSHtInfilRem + ZonePreDefRep(iZone).SHGSHtEquipAdd + ZonePreDefRep(iZone).SHGSHtEquipRem +
                            ZonePreDefRep(iZone).SHGSHtSurfHt + ZonePreDefRep(iZone).SHGSHtSurfCl;
                    total = -total; // want to know the negative value of the sum since the row should add up to zero
                    if (total > 0) {
                        ZonePreDefRep(iZone).SHGSHtOtherAdd = total;
                        ZonePreDefRep(iZone).SHGSHtOtherRem = 0.0;
                    } else {
                        ZonePreDefRep(iZone).SHGSHtOtherAdd = 0.0;
                        ZonePreDefRep(iZone).SHGSHtOtherRem = total;
                    }
                }
            } else {
                if ((ZnAirRpt(iZone).SumMCpDTsystem + radiantCool(iZone) + ZnAirRpt(iZone).SumNonAirSystem * mult) < ZonePreDefRep(iZone).clPeak) {
                    ZonePreDefRep(iZone).clPeak = ZnAirRpt(iZone).SumMCpDTsystem + radiantCool(iZone) + ZnAirRpt(iZone).SumNonAirSystem * mult;
                    // determine timestamp
                    //      ActualTimeS = CurrentTime-TimeStepZone+SysTimeElapsed
                    //      ActualtimeE = ActualTimeS+TimeStepSys
                    //      ActualTimeHrS=INT(ActualTimeS)
                    //      ActualTimeMin=NINT((ActualtimeE - ActualTimeHrS)*FracToMin)
                    ActualTimeMin = DetermineMinuteForReporting(state, t_timeStepType);
                    EncodeMonDayHrMin(timestepTimeStamp, state.dataEnvrn->Month, state.dataEnvrn->DayOfMonth, state.dataGlobal->HourOfDay, ActualTimeMin);
                    ZonePreDefRep(iZone).clPtTimeStamp = timestepTimeStamp;
                    // HVAC Input Sensible Air Heating
                    // HVAC Input Sensible Air Cooling
                    ZonePreDefRep(iZone).SHGSClHvacHt = 0.0;
                    ZonePreDefRep(iZone).SHGSClHvacCl = ZnAirRpt(iZone).SumMCpDTsystem + ZnAirRpt(iZone).SumNonAirSystem * mult;
                    // HVAC Input Heated Surface Heating
                    // HVAC Input Cooled Surface Cooling
                    ZonePreDefRep(iZone).SHGSClSurfHt = radiantHeat(iZone);
                    ZonePreDefRep(iZone).SHGSClSurfCl = radiantCool(iZone);
                    // HVAC heating by ATU at cool peak
                    // HVAC cooling by ATU at cool peak
                    ZonePreDefRep(iZone).SHGSClHvacATUHt = ATUHeat(iZone);
                    ZonePreDefRep(iZone).SHGSClHvacATUCl = ATUCool(iZone);
                    // People Sensible Heat Addition
                    ZonePreDefRep(iZone).SHGSClPeoplAdd = ZnRpt(iZone).PeopleSenGainRate * mult;
                    // Lights Sensible Heat Addition
                    ZonePreDefRep(iZone).SHGSClLiteAdd = ZnRpt(iZone).LtsTotGainRate * mult;
                    // Equipment Sensible Heat Addition
                    // Equipment Sensible Heat Removal
                    eqpSens = ZnRpt(iZone).ElecRadGainRate + ZnRpt(iZone).GasRadGainRate + ZnRpt(iZone).HWRadGainRate +
                              ZnRpt(iZone).SteamRadGainRate + ZnRpt(iZone).OtherRadGainRate + ZnRpt(iZone).ElecConGainRate +
                              ZnRpt(iZone).GasConGainRate + ZnRpt(iZone).HWConGainRate + ZnRpt(iZone).SteamConGainRate +
                              ZnRpt(iZone).OtherConGainRate;
                    if (eqpSens > 0.0) {
                        ZonePreDefRep(iZone).SHGSClEquipAdd = eqpSens * mult;
                        ZonePreDefRep(iZone).SHGSClEquipRem = 0.0;
                    } else {
                        ZonePreDefRep(iZone).SHGSClEquipAdd = 0.0;
                        ZonePreDefRep(iZone).SHGSClEquipRem = eqpSens * mult;
                    }
                    // Window Heat Addition
                    // Window Heat Removal
                    ZonePreDefRep(iZone).SHGSClWindAdd = ZoneWinHeatGainRep(iZone) * mult;
                    ZonePreDefRep(iZone).SHGSClWindRem = -ZoneWinHeatLossRep(iZone) * mult;
                    // mixing object cool addition and removal
                    if (ZnAirRpt(iZone).SumMCpDTzones > 0.0) {
                        ZonePreDefRep(iZone).SHGSClIzaAdd = ZnAirRpt(iZone).SumMCpDTzones * mult;
                        ZonePreDefRep(iZone).SHGSClIzaRem = 0.0;
                    } else {
                        ZonePreDefRep(iZone).SHGSClIzaAdd = 0.0;
                        ZonePreDefRep(iZone).SHGSClIzaRem = ZnAirRpt(iZone).SumMCpDTzones * mult;
                    }
                    // Infiltration Heat Addition
                    // Infiltration Heat Removal
                    if (ZnAirRpt(iZone).SumMCpDtInfil > 0.0) {
                        ZonePreDefRep(iZone).SHGSClInfilAdd = ZnAirRpt(iZone).SumMCpDtInfil * mult;
                        ZonePreDefRep(iZone).SHGSClInfilRem = 0.0;
                    } else {
                        ZonePreDefRep(iZone).SHGSClInfilAdd = 0.0;
                        ZonePreDefRep(iZone).SHGSClInfilRem = ZnAirRpt(iZone).SumMCpDtInfil * mult;
                    }
                    // Opaque Surface Conduction and Other Heat Addition
                    // Opaque Surface Conduction and Other Heat Removal
                    total = ZonePreDefRep(iZone).SHGSClPeoplAdd + ZonePreDefRep(iZone).SHGSClLiteAdd + ZonePreDefRep(iZone).SHGSClHvacHt +
                            ZonePreDefRep(iZone).SHGSClHvacCl + ZonePreDefRep(iZone).SHGSClIzaAdd + ZonePreDefRep(iZone).SHGSClIzaRem +
                            ZonePreDefRep(iZone).SHGSClWindAdd + ZonePreDefRep(iZone).SHGSClWindRem + ZonePreDefRep(iZone).SHGSClInfilAdd +
                            ZonePreDefRep(iZone).SHGSClInfilRem + ZonePreDefRep(iZone).SHGSClEquipAdd + ZonePreDefRep(iZone).SHGSClEquipRem +
                            ZonePreDefRep(iZone).SHGSClSurfHt + ZonePreDefRep(iZone).SHGSClSurfCl;
                    total = -total; // want to know the negative value of the sum since the row should add up to zero
                    if (total > 0) {
                        ZonePreDefRep(iZone).SHGSClOtherAdd = total;
                        ZonePreDefRep(iZone).SHGSClOtherRem = 0.0;
                    } else {
                        ZonePreDefRep(iZone).SHGSClOtherAdd = 0.0;
                        ZonePreDefRep(iZone).SHGSClOtherRem = total;
                    }
                }
            }
        }
        //------------------------------------
        // BUILDING PEAK COOLING AND HEATING
        //------------------------------------
        bldgHtPk = 0.0;
        bldgClPk = 0.0;
        for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
            mult = Zone(iZone).Multiplier * Zone(iZone).ListMultiplier;
            if ((ZnAirRpt(iZone).SumMCpDTsystem + radiantHeat(iZone) + ZnAirRpt(iZone).SumNonAirSystem * mult) > 0) {
                bldgHtPk += ZnAirRpt(iZone).SumMCpDTsystem + radiantHeat(iZone) + ZnAirRpt(iZone).SumNonAirSystem * mult;
            } else {
                bldgClPk += ZnAirRpt(iZone).SumMCpDTsystem + radiantCool(iZone) + ZnAirRpt(iZone).SumNonAirSystem * mult;
            }
        }
        if (bldgHtPk > BuildingPreDefRep.htPeak) {
            BuildingPreDefRep.htPeak = bldgHtPk;
            // determine timestamp
            //  ActualTimeS = CurrentTime-TimeStepZone+SysTimeElapsed
            //  ActualtimeE = ActualTimeS+TimeStepSys
            //  ActualTimeHrS=INT(ActualTimeS)
            //  ActualTimeMin=NINT((ActualtimeE - ActualTimeHrS)*FracToMin)
            ActualTimeMin = DetermineMinuteForReporting(state, t_timeStepType);
            EncodeMonDayHrMin(timestepTimeStamp, state.dataEnvrn->Month, state.dataEnvrn->DayOfMonth, state.dataGlobal->HourOfDay, ActualTimeMin);
            BuildingPreDefRep.htPtTimeStamp = timestepTimeStamp;
            // reset building level results to zero prior to accumulating across zones
            BuildingPreDefRep.SHGSHtHvacHt = 0.0;
            BuildingPreDefRep.SHGSHtHvacCl = 0.0;
            BuildingPreDefRep.SHGSHtHvacATUHt = 0.0;
            BuildingPreDefRep.SHGSHtHvacATUCl = 0.0;
            BuildingPreDefRep.SHGSHtSurfHt = 0.0;
            BuildingPreDefRep.SHGSHtSurfCl = 0.0;
            BuildingPreDefRep.SHGSHtPeoplAdd = 0.0;
            BuildingPreDefRep.SHGSHtLiteAdd = 0.0;
            BuildingPreDefRep.SHGSHtEquipAdd = 0.0;
            BuildingPreDefRep.SHGSHtWindAdd = 0.0;
            BuildingPreDefRep.SHGSHtIzaAdd = 0.0;
            BuildingPreDefRep.SHGSHtInfilAdd = 0.0;
            BuildingPreDefRep.SHGSHtOtherAdd = 0.0;
            BuildingPreDefRep.SHGSHtEquipRem = 0.0;
            BuildingPreDefRep.SHGSHtWindRem = 0.0;
            BuildingPreDefRep.SHGSHtIzaRem = 0.0;
            BuildingPreDefRep.SHGSHtInfilRem = 0.0;
            BuildingPreDefRep.SHGSHtOtherRem = 0.0;
            for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
                mult = Zone(iZone).Multiplier * Zone(iZone).ListMultiplier;
                // HVAC Input Sensible Air Heating
                // HVAC Input Sensible Air Cooling
                BuildingPreDefRep.SHGSHtHvacHt += ZnAirRpt(iZone).SumMCpDTsystem + ZnAirRpt(iZone).SumNonAirSystem * mult;
                // HVAC Input Heated Surface Heating
                // HVAC Input Cooled Surface Cooling
                BuildingPreDefRep.SHGSHtSurfHt += radiantHeat(iZone);
                BuildingPreDefRep.SHGSHtSurfCl += radiantCool(iZone);
                // HVAC ATU Heating
                // HVAC ATU Cooling
                BuildingPreDefRep.SHGSHtHvacATUHt += ATUHeat(iZone);
                BuildingPreDefRep.SHGSHtHvacATUCl += ATUCool(iZone);
                // People Sensible Heat Addition
                BuildingPreDefRep.SHGSHtPeoplAdd += ZnRpt(iZone).PeopleSenGainRate * mult;
                // Lights Sensible Heat Addition
                BuildingPreDefRep.SHGSHtLiteAdd += ZnRpt(iZone).LtsTotGainRate * mult;
                // Equipment Sensible Heat Addition
                // Equipment Sensible Heat Removal
                eqpSens = ZnRpt(iZone).ElecRadGainRate + ZnRpt(iZone).GasRadGainRate + ZnRpt(iZone).HWRadGainRate + ZnRpt(iZone).SteamRadGainRate +
                          ZnRpt(iZone).OtherRadGainRate + ZnRpt(iZone).ElecConGainRate + ZnRpt(iZone).GasConGainRate + ZnRpt(iZone).HWConGainRate +
                          ZnRpt(iZone).SteamConGainRate + ZnRpt(iZone).OtherConGainRate;
                if (eqpSens > 0.0) {
                    BuildingPreDefRep.SHGSHtEquipAdd += eqpSens * mult;
                } else {
                    BuildingPreDefRep.SHGSHtEquipRem += eqpSens * mult;
                }
                // Window Heat Addition
                // Window Heat Removal
                BuildingPreDefRep.SHGSHtWindAdd += ZoneWinHeatGainRep(iZone) * mult;
                BuildingPreDefRep.SHGSHtWindRem -= ZoneWinHeatLossRep(iZone) * mult;
                // mixing object heat addition and removal
                if (ZnAirRpt(iZone).SumMCpDTzones > 0.0) {
                    BuildingPreDefRep.SHGSHtIzaAdd += ZnAirRpt(iZone).SumMCpDTzones * mult;
                } else {
                    BuildingPreDefRep.SHGSHtIzaRem += ZnAirRpt(iZone).SumMCpDTzones * mult;
                }
                // Infiltration Heat Addition
                // Infiltration Heat Removal
                if (ZnAirRpt(iZone).SumMCpDtInfil > 00) {
                    BuildingPreDefRep.SHGSHtInfilAdd += ZnAirRpt(iZone).SumMCpDtInfil * mult;
                } else {
                    BuildingPreDefRep.SHGSHtInfilRem += ZnAirRpt(iZone).SumMCpDtInfil * mult;
                }
            }
            // Opaque Surface Conduction and Other Heat Addition
            // Opaque Surface Conduction and Other Heat Removal
            total = BuildingPreDefRep.SHGSHtPeoplAdd + BuildingPreDefRep.SHGSHtLiteAdd + BuildingPreDefRep.SHGSHtHvacHt +
                    BuildingPreDefRep.SHGSHtHvacCl + BuildingPreDefRep.SHGSHtIzaAdd + BuildingPreDefRep.SHGSHtIzaRem +
                    BuildingPreDefRep.SHGSHtWindAdd + BuildingPreDefRep.SHGSHtWindRem + BuildingPreDefRep.SHGSHtInfilAdd +
                    BuildingPreDefRep.SHGSHtInfilRem + BuildingPreDefRep.SHGSHtEquipAdd + BuildingPreDefRep.SHGSHtEquipRem +
                    BuildingPreDefRep.SHGSHtSurfHt + BuildingPreDefRep.SHGSHtSurfCl;
            total = -total; // want to know the negative value of the sum since the row should add up to zero
            if (total > 0) {
                BuildingPreDefRep.SHGSHtOtherAdd += total;
            } else {
                BuildingPreDefRep.SHGSHtOtherRem += total;
            }
        }
        if (bldgClPk < BuildingPreDefRep.clPeak) {
            BuildingPreDefRep.clPeak = bldgClPk;
            // determine timestamp
            //  ActualTimeS = CurrentTime-TimeStepZone+SysTimeElapsed
            //  ActualtimeE = ActualTimeS+TimeStepSys
            //  ActualTimeHrS=INT(ActualTimeS)
            //  ActualTimeMin=NINT((ActualtimeE - ActualTimeHrS)*FracToMin)
            ActualTimeMin = DetermineMinuteForReporting(state, t_timeStepType);
            EncodeMonDayHrMin(timestepTimeStamp, state.dataEnvrn->Month, state.dataEnvrn->DayOfMonth, state.dataGlobal->HourOfDay, ActualTimeMin);
            BuildingPreDefRep.clPtTimeStamp = timestepTimeStamp;
            // reset building level results to zero prior to accumulating across zones
            BuildingPreDefRep.SHGSClHvacHt = 0.0;
            BuildingPreDefRep.SHGSClHvacCl = 0.0;
            BuildingPreDefRep.SHGSClSurfHt = 0.0;
            BuildingPreDefRep.SHGSClSurfCl = 0.0;
            BuildingPreDefRep.SHGSClHvacATUHt = 0.0;
            BuildingPreDefRep.SHGSClHvacATUCl = 0.0;
            BuildingPreDefRep.SHGSClPeoplAdd = 0.0;
            BuildingPreDefRep.SHGSClLiteAdd = 0.0;
            BuildingPreDefRep.SHGSClEquipAdd = 0.0;
            BuildingPreDefRep.SHGSClWindAdd = 0.0;
            BuildingPreDefRep.SHGSClIzaAdd = 0.0;
            BuildingPreDefRep.SHGSClInfilAdd = 0.0;
            BuildingPreDefRep.SHGSClOtherAdd = 0.0;
            BuildingPreDefRep.SHGSClEquipRem = 0.0;
            BuildingPreDefRep.SHGSClWindRem = 0.0;
            BuildingPreDefRep.SHGSClIzaRem = 0.0;
            BuildingPreDefRep.SHGSClInfilRem = 0.0;
            BuildingPreDefRep.SHGSClOtherRem = 0.0;
            for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
                mult = Zone(iZone).Multiplier * Zone(iZone).ListMultiplier;
                // HVAC Input Sensible Air Heating
                // HVAC Input Sensible Air Cooling
                BuildingPreDefRep.SHGSClHvacCl += ZnAirRpt(iZone).SumMCpDTsystem + ZnAirRpt(iZone).SumNonAirSystem * mult;
                // HVAC Input Heated Surface Heating
                // HVAC Input Cooled Surface Cooling
                BuildingPreDefRep.SHGSClSurfHt += radiantHeat(iZone);
                BuildingPreDefRep.SHGSClSurfCl += radiantCool(iZone);
                // HVAC ATU Heating
                // HVAC ATU Cooling
                BuildingPreDefRep.SHGSClHvacATUHt += ATUHeat(iZone);
                BuildingPreDefRep.SHGSClHvacATUCl += ATUCool(iZone);
                // People Sensible Heat Addition
                BuildingPreDefRep.SHGSClPeoplAdd += ZnRpt(iZone).PeopleSenGainRate * mult;
                // Lights Sensible Heat Addition
                BuildingPreDefRep.SHGSClLiteAdd += ZnRpt(iZone).LtsTotGainRate * mult;
                // Equipment Sensible Heat Addition
                // Equipment Sensible Heat Removal
                eqpSens = ZnRpt(iZone).ElecRadGainRate + ZnRpt(iZone).GasRadGainRate + ZnRpt(iZone).HWRadGainRate + ZnRpt(iZone).SteamRadGainRate +
                          ZnRpt(iZone).OtherRadGainRate + ZnRpt(iZone).ElecConGainRate + ZnRpt(iZone).GasConGainRate + ZnRpt(iZone).HWConGainRate +
                          ZnRpt(iZone).SteamConGainRate + ZnRpt(iZone).OtherConGainRate;
                if (eqpSens > 0.0) {
                    BuildingPreDefRep.SHGSClEquipAdd += eqpSens * mult;
                } else {
                    BuildingPreDefRep.SHGSClEquipRem += eqpSens * mult;
                }
                // Window Heat Addition
                // Window Heat Removal
                BuildingPreDefRep.SHGSClWindAdd += ZoneWinHeatGainRep(iZone) * mult;
                BuildingPreDefRep.SHGSClWindRem -= ZoneWinHeatLossRep(iZone) * mult;
                // mixing object cool addition and removal
                if (ZnAirRpt(iZone).SumMCpDTzones > 0.0) {
                    BuildingPreDefRep.SHGSClIzaAdd += ZnAirRpt(iZone).SumMCpDTzones * mult;
                } else {
                    BuildingPreDefRep.SHGSClIzaRem += ZnAirRpt(iZone).SumMCpDTzones * mult;
                }
                // Infiltration Heat Addition
                // Infiltration Heat Removal
                if (ZnAirRpt(iZone).SumMCpDtInfil > 00) {
                    BuildingPreDefRep.SHGSClInfilAdd += ZnAirRpt(iZone).SumMCpDtInfil * mult;
                } else {
                    BuildingPreDefRep.SHGSClInfilRem += ZnAirRpt(iZone).SumMCpDtInfil * mult;
                }
            }
            // Opaque Surface Conduction and Other Heat Addition
            // Opaque Surface Conduction and Other Heat Removal
            total = BuildingPreDefRep.SHGSClPeoplAdd + BuildingPreDefRep.SHGSClLiteAdd + BuildingPreDefRep.SHGSClHvacHt +
                    BuildingPreDefRep.SHGSClHvacCl + BuildingPreDefRep.SHGSClIzaAdd + BuildingPreDefRep.SHGSClIzaRem +
                    BuildingPreDefRep.SHGSClWindAdd + BuildingPreDefRep.SHGSClWindRem + BuildingPreDefRep.SHGSClInfilAdd +
                    BuildingPreDefRep.SHGSClInfilRem + BuildingPreDefRep.SHGSClEquipAdd + BuildingPreDefRep.SHGSClEquipRem +
                    BuildingPreDefRep.SHGSClSurfHt + BuildingPreDefRep.SHGSClSurfCl;
            total = -total; // want to know the negative value of the sum since the row should add up to zero
            if (total > 0) {
                BuildingPreDefRep.SHGSClOtherAdd += total;
            } else {
                BuildingPreDefRep.SHGSClOtherRem += total;
            }
        }
    }

    //======================================================================================================================
    //======================================================================================================================

    //    WRITE OUTPUT FILE ROUTINES

    //======================================================================================================================
    //======================================================================================================================

    void WriteTabularReports(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   August 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   This routine hides from the main simulation that four specific
        //   types of tabular reports are each created. If another type of
        //   report is added it can be added to the list here.

        FillWeatherPredefinedEntries(state);
        FillRemainingPredefinedEntries(state);
        auto &ort(state.dataOutRptTab);

        if (ort->WriteTabularFiles) {

            //iUnitsStyle unitsql_unitconv = iUnitsStyle::None;
            ort->unitsStyle_SQLite = iUnitsStyle::None;
            // if (unitsqltab == 5) unitsqltab = ort->unitsStyle; // This is the default UseOutputControlTableStyles
            if (unitSQLiteTable == 0) {
                ort->unitsStyle_SQLite = iUnitsStyle::None;
            } else if (unitSQLiteTable == 1) {
                ort->unitsStyle_SQLite = iUnitsStyle::JtoKWH;
            } else if (unitSQLiteTable == 2) {
                ort->unitsStyle_SQLite = iUnitsStyle::JtoMJ;
            } else if (unitSQLiteTable == 3) {
                ort->unitsStyle_SQLite = iUnitsStyle::JtoGJ;
            } else if (unitSQLiteTable == 4) {
                ort->unitsStyle_SQLite = iUnitsStyle::InchPound;
            } else {
                ort->unitsStyle_SQLite = ort->unitsStyle; // This is the default UseOutputControlTableStyles
            }

            // call each type of report in turn
            WriteBEPSTable(state);
            WriteTableOfContents(state);
            WriteVeriSumTable(state);
            WriteDemandEndUseSummary(state);
            WriteSourceEnergyEndUseSummary(state);
            WriteComponentSizing(state);
            WriteSurfaceShadowing(state);
            WriteCompCostTable(state);
            WriteAdaptiveComfortTable(state);
            WriteEioTables(state);
            WriteLoadComponentSummaryTables(state);
            WriteHeatEmissionTable(state);

            if (ort->displayThermalResilienceSummary) WriteThermalResilienceTables(state);
            if (ort->displayCO2ResilienceSummary) WriteCO2ResilienceTables(state);
            if (ort->displayVisualResilienceSummary) WriteVisualResilienceTables(state);

            coilSelectionReportObj->finishCoilSummaryReportTable(state); // call to write out the coil selection summary table data
            WritePredefinedTables(state);                                // moved to come after zone load components is finished

            if (state.dataGlobal->DoWeathSim) {
                WriteMonthlyTables(state);
                WriteTimeBinTables(state);
                OutputReportTabularAnnual::WriteAnnualTables(state);
            }
        }

        constexpr static auto variable_fmt{" {}={:12}\n"};
        state.files.audit.ensure_open(state, "WriteTabularReports", state.files.outputControl.audit);
        print(state.files.audit, variable_fmt, "MonthlyInputCount", ort->MonthlyInputCount);
        print(state.files.audit, variable_fmt, "sizeMonthlyInput", ort->sizeMonthlyInput);
        print(state.files.audit, variable_fmt, "MonthlyFieldSetInputCount", ort->MonthlyFieldSetInputCount);
        print(state.files.audit, variable_fmt, "sizeMonthlyFieldSetInput", ort->sizeMonthlyFieldSetInput);
        print(state.files.audit, variable_fmt, "MonthlyTablesCount", ort->MonthlyTablesCount);
        print(state.files.audit, variable_fmt, "MonthlyColumnsCount", ort->MonthlyColumnsCount);
        print(state.files.audit, variable_fmt, "sizeReportName", state.dataOutRptPredefined->sizeReportName);
        print(state.files.audit, variable_fmt, "numReportName", state.dataOutRptPredefined->numReportName);
        print(state.files.audit, variable_fmt, "sizeSubTable", state.dataOutRptPredefined->sizeSubTable);
        print(state.files.audit, variable_fmt, "numSubTable", state.dataOutRptPredefined->numSubTable);
        print(state.files.audit, variable_fmt, "sizeColumnTag", state.dataOutRptPredefined->sizeColumnTag);
        print(state.files.audit, variable_fmt, "numColumnTag", state.dataOutRptPredefined->numColumnTag);
        print(state.files.audit, variable_fmt, "sizeTableEntry", state.dataOutRptPredefined->sizeTableEntry);
        print(state.files.audit, variable_fmt, "numTableEntry", state.dataOutRptPredefined->numTableEntry);
        print(state.files.audit, variable_fmt, "sizeCompSizeTableEntry", state.dataOutRptPredefined->sizeCompSizeTableEntry);
        print(state.files.audit, variable_fmt, "numCompSizeTableEntry", state.dataOutRptPredefined->numCompSizeTableEntry);
    }

    void parseStatLine(const std::string &lineIn,
                       StatLineType &lineType,
                       bool &desConditionlinepassed,
                       bool &heatingDesignlinepassed,
                       bool &coolingDesignlinepassed,
                       bool &isKoppen)
    {
        // assumes that all the variables are initialized outside of this routine -- it does not re-initialize them
        if (has_prefix(lineIn, "Statistics")) {
            lineType = StatLineType::StatisticsLine;
        } else if (has_prefix(lineIn, "Location")) {
            lineType = StatLineType::LocationLine;
        } else if (has_prefix(lineIn, "{")) {
            lineType = StatLineType::LatLongLine;
        } else if (has_prefix(lineIn, "Elevation")) {
            lineType = StatLineType::ElevationLine;
        } else if (has_prefix(lineIn, "Standard Pressure")) {
            lineType = StatLineType::StdPressureLine;
        } else if (has_prefix(lineIn, "Data Source")) {
            lineType = StatLineType::DataSourceLine;
        } else if (has_prefix(lineIn, "WMO Station")) {
            lineType = StatLineType::WMOStationLine;
        } else if (has(lineIn, "Design Conditions")) {
            if (!desConditionlinepassed) {
                desConditionlinepassed = true;
                lineType = StatLineType::DesignConditionsLine;
            }
        } else if (has_prefix(lineIn, "\tHeating")) {
            if (!heatingDesignlinepassed) {
                heatingDesignlinepassed = true;
                lineType = StatLineType::heatingConditionsLine;
            }
        } else if (has_prefix(lineIn, "\tCooling")) {
            if (!coolingDesignlinepassed) {
                coolingDesignlinepassed = true;
                lineType = StatLineType::coolingConditionsLine;
            }
        } else if (has(lineIn, "(standard) heating degree-days (18.3")) {
            lineType = StatLineType::stdHDDLine;
        } else if (has(lineIn, "(standard) cooling degree-days (10")) {
            lineType = StatLineType::stdCDDLine;

        } else if (has(lineIn, "Maximum Dry Bulb")) {
            lineType = StatLineType::maxDryBulbLine;
        } else if (has(lineIn, "Minimum Dry Bulb")) {
            lineType = StatLineType::minDryBulbLine;
        } else if (has(lineIn, "Maximum Dew Point")) {
            lineType = StatLineType::maxDewPointLine;
        } else if (has(lineIn, "Minimum Dew Point")) {
            lineType = StatLineType::minDewPointLine;
        } else if (has(lineIn, "(wthr file) heating degree-days (18") || has(lineIn, "heating degree-days (18")) {
            lineType = StatLineType::wthHDDLine;
        } else if (has(lineIn, "(wthr file) cooling degree-days (10") || has(lineIn, "cooling degree-days (10")) {
            lineType = StatLineType::wthCDDLine;
        }
        // these not part of big if/else because sequential
        if (lineType == StatLineType::KoppenDes1Line && isKoppen) lineType = StatLineType::KoppenDes2Line;
        if (lineType == StatLineType::KoppenLine && isKoppen) lineType = StatLineType::KoppenDes1Line;
        if (has(lineIn, "ppen classification)")) lineType = StatLineType::KoppenLine;
        if (lineType == StatLineType::AshStdDes2Line) lineType = StatLineType::AshStdDes3Line;
        if (lineType == StatLineType::AshStdDes1Line) lineType = StatLineType::AshStdDes2Line;
        if (lineType == StatLineType::AshStdLine) lineType = StatLineType::AshStdDes1Line;
        if (has(lineIn, "ASHRAE Standard")) lineType = StatLineType::AshStdLine;
    }

    void FillWeatherPredefinedEntries(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   Feb 2008
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Read the STAT file for the active weather file and summarize in a predefined report.
        //   The stat file that is attached may have several formats -- from evolution of the
        //   stat file from the weather converter (or others that produce a similar stat file).

        // Using/Aliasing
        using namespace OutputReportPredefined;

        // SUBROUTINE PARAMETER DEFINITIONS:
        static std::string const degChar("°");

        auto &ort(state.dataOutRptTab);

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:

        StatLineType lineType = StatLineType::Initialized;
        StatLineType lineTypeinterim = StatLineType::Initialized;
        bool isASHRAE;
        bool iscalc;
        bool isKoppen;
        std::string::size_type ashPtr;
        std::string::size_type lnPtr;
        int col1;
        int col2;
        int col3;
        std::string::size_type sposlt;
        std::string::size_type eposlt;
        std::string::size_type sposlg;
        std::string::size_type eposlg;
        std::string::size_type spostz;
        std::string::size_type epostz;
        std::string ashDesYear;
        std::string ashZone; // ashrae climate zone
        std::string curNameWithSIUnits;
        std::string curNameAndUnits;
        int indexUnitConv;
        std::string storeASHRAEHDD;
        std::string storeASHRAECDD;
        bool heatingDesignlinepassed;
        bool coolingDesignlinepassed;
        bool desConditionlinepassed;

        isASHRAE = false;
        iscalc = false;
        isKoppen = false;
        heatingDesignlinepassed = false;
        coolingDesignlinepassed = false;
        desConditionlinepassed = false;
        storeASHRAEHDD = "";
        storeASHRAECDD = "";
        lineTypeinterim = StatLineType::Initialized;
        if (FileSystem::fileExists(state.files.inStatFileName.fileName)) {
            auto statFile = state.files.inStatFileName.open(state, "FillWeatherPredefinedEntries");
            while (statFile.good()) { // end of file, or error
                lineType = lineTypeinterim;
                auto lineIn = statFile.readLine().data;
                // reconcile line with different versions of stat file
                // v7.1 added version as first line.
                strip(lineIn);
                parseStatLine(lineIn, lineType, desConditionlinepassed, heatingDesignlinepassed, coolingDesignlinepassed, isKoppen);

                {
                    auto const SELECT_CASE_var(lineType);
                    if (SELECT_CASE_var == StatLineType::StatisticsLine) { // Statistics for USA_CA_San.Francisco_TMY2
                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Reference", lineIn.substr(15));
                    } else if (SELECT_CASE_var == StatLineType::LocationLine) { // Location -- SAN_FRANCISCO CA USA
                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Site:Location", lineIn.substr(11));
                    } else if (SELECT_CASE_var == StatLineType::LatLongLine) { //      {N 37° 37'} {W 122° 22'} {GMT -8.0 Hours}
                        // find the {}
                        sposlt = index(lineIn, '{');
                        eposlt = index(lineIn, '}');
                        if (sposlt != std::string::npos && eposlt != std::string::npos) {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Latitude", lineIn.substr(sposlt, eposlt - sposlt + 1));
                            // redefine so next scan can go with {}
                            lineIn[sposlt] = '[';
                            lineIn[eposlt] = ']';
                        } else {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Latitude", "not found");
                        }
                        sposlg = index(lineIn, '{');
                        eposlg = index(lineIn, '}');
                        if (sposlg != std::string::npos && eposlg != std::string::npos) {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Longitude", lineIn.substr(sposlg, eposlg - sposlg + 1));
                            // redefine so next scan can go with {}
                            lineIn[sposlg] = '[';
                            lineIn[eposlg] = ']';
                        } else {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Longitude", "not found");
                        }
                        spostz = index(lineIn, '{');
                        epostz = index(lineIn, '}');
                        if (spostz != std::string::npos && epostz != std::string::npos) {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Time Zone", lineIn.substr(spostz, epostz - spostz + 1));
                            // redefine so next scan can go with {}
                            lineIn[spostz] = '[';
                            lineIn[epostz] = ']';
                        } else {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Time Zone", "not found");
                        }
                    } else if (SELECT_CASE_var == StatLineType::ElevationLine) { // Elevation --     5m above sea level
                        lnPtr = index(lineIn.substr(12), 'm');
                        if (lnPtr != std::string::npos) {
                            curNameWithSIUnits = "Elevation (m) " + lineIn.substr(12 + lnPtr + 2);
                            if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                                PreDefTableEntry(state,
                                    state.dataOutRptPredefined->pdchWthrVal, curNameAndUnits, RealToStr(ConvertIP(state, indexUnitConv, StrToReal(lineIn.substr(12, lnPtr))), 1));
                            } else {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, curNameWithSIUnits, lineIn.substr(12, lnPtr));
                            }
                        } else {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Elevation", "not found");
                        }
                    } else if (SELECT_CASE_var == StatLineType::StdPressureLine) { // Standard Pressure at Elevation -- 101265Pa
                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Standard Pressure at Elevation", lineIn.substr(34));
                    } else if (SELECT_CASE_var == StatLineType::DataSourceLine) { // Data Source -- TMY2-23234
                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Data Source", lineIn.substr(15));
                    } else if (SELECT_CASE_var == StatLineType::WMOStationLine) { // WMO Station 724940
                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "WMO Station", lineIn.substr(12));
                    } else if (SELECT_CASE_var ==
                               StatLineType::DesignConditionsLine) { //  - Using Design Conditions from "Climate Design Data 2005 ASHRAE Handbook"
                        ashPtr = index(lineIn, "ASHRAE");
                        if (ashPtr != std::string::npos) {
                            isASHRAE = true;
                            iscalc = true;
                            if (ashPtr > 4u) { // Autodesk:BoundsViolation IF block added to protect against ashPtr<=5
                                ashDesYear = lineIn.substr(ashPtr - 5, 5);
                            } else {
                                ashDesYear = "";
                            }
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Weather File Design Conditions", "Climate Design Data " + ashDesYear + "ASHRAE Handbook");
                        } else if (has(lineIn, "not calculated") || lineIn == "") {
                            iscalc = false;
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Weather File Design Conditions", "not calculated, Number of days < 1 year");
                        } else {
                            isASHRAE = false;
                            iscalc = true;
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Weather File Design Conditions", "Calculated from the weather file");
                        }
                    } else if (SELECT_CASE_var == StatLineType::heatingConditionsLine) { //  winter/heating design conditions
                        if (iscalc) {
                            if (isASHRAE) {
                                if (ashDesYear == "2001") {
                                    if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                        curNameWithSIUnits = "Heating Design Temperature 99.6% (C)";
                                        LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                         curNameAndUnits,
                                                         RealToStr(ConvertIP(state, indexUnitConv, StrToReal(GetColumnUsingTabs(lineIn, 2))), 1) + degChar);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                         "Heating Design Temperature 99% (F)",
                                                         RealToStr(ConvertIP(state, indexUnitConv, StrToReal(GetColumnUsingTabs(lineIn, 3))), 1) + degChar);
                                    } else {
                                        PreDefTableEntry(state,
                                            state.dataOutRptPredefined->pdchWthrVal, "Heating Design Temperature 99.6% (C)", GetColumnUsingTabs(lineIn, 2) + degChar);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Heating Design Temperature 99% (C)", GetColumnUsingTabs(lineIn, 3) + degChar);
                                    }
                                } else { // 2005 and 2009 are the same
                                    if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                        curNameWithSIUnits = "Heating Design Temperature 99.6% (C)";
                                        LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                         curNameAndUnits,
                                                         RealToStr(ConvertIP(state, indexUnitConv, StrToReal(GetColumnUsingTabs(lineIn, 4))), 1) + degChar);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                         "Heating Design Temperature 99% (F)",
                                                         RealToStr(ConvertIP(state, indexUnitConv, StrToReal(GetColumnUsingTabs(lineIn, 5))), 1) + degChar);
                                    } else {
                                        PreDefTableEntry(state,
                                            state.dataOutRptPredefined->pdchWthrVal, "Heating Design Temperature 99.6% (C)", GetColumnUsingTabs(lineIn, 4) + degChar);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Heating Design Temperature 99% (C)", GetColumnUsingTabs(lineIn, 5) + degChar);
                                    }
                                }
                            } else { // from weather file
                                if (is_blank(GetColumnUsingTabs(lineIn, 5))) {
                                    col1 = 3;
                                    col2 = 4;
                                } else {
                                    col1 = 4;
                                    col2 = 5;
                                }
                                if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                    curNameWithSIUnits = "Heating Design Temperature 99.6% (C)";
                                    LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                     curNameAndUnits,
                                                     RealToStr(ConvertIP(state, indexUnitConv, StrToReal(GetColumnUsingTabs(lineIn, col1))), 1) + degChar);
                                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                     "Heating Design Temperature 99% (F)",
                                                     RealToStr(ConvertIP(state, indexUnitConv, StrToReal(GetColumnUsingTabs(lineIn, col2))), 1) + degChar);
                                } else {
                                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Heating Design Temperature 99.6% (C)", GetColumnUsingTabs(lineIn, col1) + degChar);
                                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Heating Design Temperature 99% (C)", GetColumnUsingTabs(lineIn, col2) + degChar);
                                }
                            }
                        }
                    } else if (SELECT_CASE_var == StatLineType::coolingConditionsLine) { //  summer/cooling design conditions
                        if (iscalc) {
                            if (isASHRAE) {
                                if (ashDesYear == "2001") {
                                    if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                        curNameWithSIUnits = "Cooling Design Temperature 0.4% (C)";
                                        LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                         curNameAndUnits,
                                                         RealToStr(ConvertIP(state, indexUnitConv, StrToReal(GetColumnUsingTabs(lineIn, 2))), 1) + degChar);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                         "Cooling Design Temperature 1% (F)",
                                                         RealToStr(ConvertIP(state, indexUnitConv, StrToReal(GetColumnUsingTabs(lineIn, 4))), 1) + degChar);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                         "Cooling Design Temperature 2% (F)",
                                                         RealToStr(ConvertIP(state, indexUnitConv, StrToReal(GetColumnUsingTabs(lineIn, 6))), 1) + degChar);
                                    } else {
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Cooling Design Temperature 0.4% (C)", GetColumnUsingTabs(lineIn, 2) + degChar);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Cooling Design Temperature 1% (C)", GetColumnUsingTabs(lineIn, 4) + degChar);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Cooling Design Temperature 2% (C)", GetColumnUsingTabs(lineIn, 6) + degChar);
                                    }
                                } else { // 2005 and 2009 are the same
                                    if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                        curNameWithSIUnits = "Cooling Design Temperature 0.4% (C)";
                                        LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                         curNameAndUnits,
                                                         RealToStr(ConvertIP(state, indexUnitConv, StrToReal(GetColumnUsingTabs(lineIn, 5))), 1) + degChar);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                         "Cooling Design Temperature 1% (F)",
                                                         RealToStr(ConvertIP(state, indexUnitConv, StrToReal(GetColumnUsingTabs(lineIn, 7))), 1) + degChar);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                         "Cooling Design Temperature 2% (F)",
                                                         RealToStr(ConvertIP(state, indexUnitConv, StrToReal(GetColumnUsingTabs(lineIn, 9))), 1) + degChar);
                                    } else {
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Cooling Design Temperature 0.4% (C)", GetColumnUsingTabs(lineIn, 5) + degChar);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Cooling Design Temperature 1% (C)", GetColumnUsingTabs(lineIn, 7) + degChar);
                                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Cooling Design Temperature 2% (C)", GetColumnUsingTabs(lineIn, 9) + degChar);
                                    }
                                }
                            } else { // from weather file
                                if (is_blank(GetColumnUsingTabs(lineIn, 6))) {
                                    col1 = 3;
                                    col2 = 4;
                                    col3 = 5;
                                } else {
                                    col1 = 4;
                                    col2 = 5;
                                    col3 = 6;
                                }
                                if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                    curNameWithSIUnits = "Cooling Design Temperature 0.4% (C)";
                                    LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                     curNameAndUnits,
                                                     RealToStr(ConvertIP(state, indexUnitConv, StrToReal(GetColumnUsingTabs(lineIn, col1))), 1) + degChar);
                                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                     "Cooling Design Temperature 1% (F)",
                                                     RealToStr(ConvertIP(state, indexUnitConv, StrToReal(GetColumnUsingTabs(lineIn, col2))), 1) + degChar);
                                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                     "Cooling Design Temperature 2% (F)",
                                                     RealToStr(ConvertIP(state, indexUnitConv, StrToReal(GetColumnUsingTabs(lineIn, col3))), 1) + degChar);
                                } else {
                                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Cooling Design Temperature 0.4% (C)", GetColumnUsingTabs(lineIn, col1) + degChar);
                                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Cooling Design Temperature 1% (C)", GetColumnUsingTabs(lineIn, col2) + degChar);
                                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Cooling Design Temperature 2% (C)", GetColumnUsingTabs(lineIn, col3) + degChar);
                                }
                            }
                        }
                    } else if (SELECT_CASE_var == StatLineType::stdHDDLine) { //  - 1745 annual (standard) heating degree-days (10°C baseline)
                        storeASHRAEHDD = lineIn.substr(2, 4);
                    } else if (SELECT_CASE_var == StatLineType::stdCDDLine) { //  -  464 annual (standard) cooling degree-days (18.3°C baseline)
                        storeASHRAECDD = lineIn.substr(2, 4);
                    } else if (SELECT_CASE_var == StatLineType::maxDryBulbLine) { //   - Maximum Dry Bulb temperature of  35.6°C on Jul  9
                        sposlt = index(lineIn, "of");
                        eposlt = index(lineIn, 'C');
                        sposlt += 2;
                        auto deg_index = index(lineIn, degChar);
                        if (deg_index != std::string::npos) {
                            eposlt = deg_index - 1;
                        } else {
                            eposlt -= 2;
                        }
                        if (sposlt != std::string::npos && eposlt != std::string::npos) {
                            if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                curNameWithSIUnits = "Maximum Dry Bulb Temperature (C)";
                                LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                 curNameAndUnits,
                                                 RealToStr(ConvertIP(state, indexUnitConv, StrToReal(lineIn.substr(sposlt, eposlt - sposlt + 1))), 1) +
                                                     degChar);
                            } else {
                                PreDefTableEntry(state,
                                    state.dataOutRptPredefined->pdchWthrVal, "Maximum Dry Bulb Temperature (C)", lineIn.substr(sposlt, eposlt - sposlt + 1) + degChar);
                            }
                        } else {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Maximum Dry Bulb Temperature", "not found");
                        }
                        sposlt = index(lineIn, "on");
                        sposlt += 2;
                        if (sposlt != std::string::npos) {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Maximum Dry Bulb Occurs on", lineIn.substr(sposlt));
                        } else {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Maximum Dry Bulb Occurs on", "not found");
                        }
                    } else if (SELECT_CASE_var == StatLineType::minDryBulbLine) { //   - Minimum Dry Bulb temperature of -22.8°C on Jan  7
                        sposlt = index(lineIn, "of");
                        eposlt = index(lineIn, 'C');
                        sposlt += 2;
                        auto deg_index = index(lineIn, degChar);
                        if (deg_index != std::string::npos) {
                            eposlt = deg_index - 1;
                        } else {
                            eposlt -= 2;
                        }
                        if (sposlt != std::string::npos && eposlt != std::string::npos) {
                            if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                curNameWithSIUnits = "Minimum Dry Bulb Temperature (C)";
                                LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                 curNameAndUnits,
                                                 RealToStr(ConvertIP(state, indexUnitConv, StrToReal(lineIn.substr(sposlt, eposlt - sposlt + 1))), 1) +
                                                     degChar);
                            } else {
                                PreDefTableEntry(state,
                                    state.dataOutRptPredefined->pdchWthrVal, "Minimum Dry Bulb Temperature (C)", lineIn.substr(sposlt, eposlt - sposlt + 1) + degChar);
                            }
                        } else {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Minimum Dry Bulb Temperature", "not found");
                        }
                        sposlt = index(lineIn, "on");
                        sposlt += 2;
                        if (sposlt != std::string::npos) {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Minimum Dry Bulb Occurs on", lineIn.substr(sposlt));
                        } else {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Minimum Dry Bulb Occurs on", "not found");
                        }
                    } else if (SELECT_CASE_var == StatLineType::maxDewPointLine) { //   - Maximum Dew Point temperature of  25.6°C on Aug  4
                        sposlt = index(lineIn, "of");
                        eposlt = index(lineIn, 'C');
                        sposlt += 2;
                        auto deg_index = index(lineIn, degChar);
                        if (deg_index != std::string::npos) {
                            eposlt = deg_index - 1;
                        } else {
                            eposlt -= 2;
                        }
                        if (sposlt != std::string::npos && eposlt != std::string::npos) {
                            if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                curNameWithSIUnits = "Maximum Dew Point Temperature (C)";
                                LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                 curNameAndUnits,
                                                 RealToStr(ConvertIP(state, indexUnitConv, StrToReal(lineIn.substr(sposlt, eposlt - sposlt + 1))), 1) +
                                                     degChar);
                            } else {
                                PreDefTableEntry(state,
                                    state.dataOutRptPredefined->pdchWthrVal, "Maximum Dew Point Temperature (C)", lineIn.substr(sposlt, eposlt - sposlt + 1) + degChar);
                            }
                        } else {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Maximum Dew Point Temperature", "not found");
                        }
                        sposlt = index(lineIn, "on");
                        sposlt += 2;
                        if (sposlt != std::string::npos) {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Maximum Dew Point Occurs on", lineIn.substr(sposlt));
                        } else {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Maximum Dew Point Occurs on", "not found");
                        }
                    } else if (SELECT_CASE_var == StatLineType::minDewPointLine) { //   - Minimum Dew Point temperature of -28.9°C on Dec 31
                        sposlt = index(lineIn, "of");
                        eposlt = index(lineIn, 'C');
                        sposlt += 2;
                        auto deg_index = index(lineIn, degChar);
                        if (deg_index != std::string::npos) {
                            eposlt = deg_index - 1;
                        } else {
                            eposlt -= 2;
                        }
                        if (sposlt != std::string::npos && eposlt != std::string::npos) {
                            if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                curNameWithSIUnits = "Minimum Dew Point Temperature (C)";
                                LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal,
                                                 curNameAndUnits,
                                                 RealToStr(ConvertIP(state, indexUnitConv, StrToReal(lineIn.substr(sposlt, eposlt - sposlt + 1))), 1) +
                                                     degChar);
                            } else {
                                PreDefTableEntry(state,
                                    state.dataOutRptPredefined->pdchWthrVal, "Minimum Dew Point Temperature (C)", lineIn.substr(sposlt, eposlt - sposlt + 1) + degChar);
                            }
                        } else {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Minimum Dew Point Temperature", "not found");
                        }
                        sposlt = index(lineIn, "on");
                        sposlt += 2;
                        if (sposlt != std::string::npos) {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Minimum Dew Point Occurs on", lineIn.substr(sposlt));
                        } else {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Minimum Dew Point Occurs on", "not found");
                        }
                    } else if (SELECT_CASE_var == StatLineType::wthHDDLine) { //  - 1745 (wthr file) annual heating degree-days (10°C baseline)
                        if (storeASHRAEHDD != "") {
                            if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                curNameWithSIUnits = "ASHRAE Handbook 2009 Heating Degree-Days - base 65°(C)";
                                LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                                PreDefTableEntry(state,
                                    state.dataOutRptPredefined->pdchWthrVal, curNameAndUnits, RealToStr(ConvertIPdelta(state, indexUnitConv, StrToReal(storeASHRAEHDD)), 1));
                            } else {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Handbook 2009 Heating Degree-Days (base 18.3°C)", storeASHRAEHDD);
                            }
                        } else {
                            if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Handbook 2009 Heating Degree-Days (base 65°F)", "not found");
                            } else {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Handbook 2009 Heating Degree-Days (base 18.3°C)", "not found");
                            }
                        }
                        if (ort->unitsStyle == iUnitsStyle::InchPound) {
                            curNameWithSIUnits = "Weather File Heating Degree-Days - base 65°(C)";
                            LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                            PreDefTableEntry(state,
                                state.dataOutRptPredefined->pdchWthrVal, curNameAndUnits, RealToStr(ConvertIPdelta(state, indexUnitConv, StrToReal(lineIn.substr(2, 4))), 1));
                            PreDefTableEntry(state,
                                state.dataOutRptPredefined->pdchLeedGenData, "Heating Degree Days", RealToStr(ConvertIPdelta(state, indexUnitConv, StrToReal(lineIn.substr(2, 4))), 1));
                        } else {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Weather File Heating Degree-Days (base 18°C)", lineIn.substr(2, 4));
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedGenData, "Heating Degree Days", lineIn.substr(2, 4));
                        }
                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedGenData, "HDD and CDD data source", "Weather File Stat");
                    } else if (SELECT_CASE_var == StatLineType::wthCDDLine) { //  -  464 (wthr file) annual cooling degree-days (18°C baseline)
                        if (storeASHRAECDD != "") {
                            if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                curNameWithSIUnits = "ASHRAE Handbook 2009  Cooling Degree-Days - base 50°(C)";
                                LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                                PreDefTableEntry(state,
                                    state.dataOutRptPredefined->pdchWthrVal, curNameAndUnits, RealToStr(ConvertIPdelta(state, indexUnitConv, StrToReal(storeASHRAECDD)), 1));
                            } else {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Handbook 2009  Cooling Degree-Days (base 10°C)", storeASHRAECDD);
                            }
                        } else {
                            if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Handbook 2009  Cooling Degree-Days (base 50°F)", "not found");
                            } else {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Handbook 2009  Cooling Degree-Days (base 10°C)", "not found");
                            }
                        }
                        if (ort->unitsStyle == iUnitsStyle::InchPound) {
                            curNameWithSIUnits = "Weather File Cooling Degree-Days - base 50°(C)";
                            LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                            PreDefTableEntry(state,
                                state.dataOutRptPredefined->pdchWthrVal, curNameAndUnits, RealToStr(ConvertIPdelta(state, indexUnitConv, StrToReal(lineIn.substr(2, 4))), 1));
                            PreDefTableEntry(state,
                                state.dataOutRptPredefined->pdchLeedGenData, "Cooling Degree Days", RealToStr(ConvertIPdelta(state, indexUnitConv, StrToReal(lineIn.substr(2, 4))), 1));
                        } else {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Weather File Cooling Degree-Days (base 10°C)", lineIn.substr(2, 4));
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedGenData, "Cooling Degree Days", lineIn.substr(2, 4));
                        }
                    } else if (SELECT_CASE_var == StatLineType::KoppenLine) { // - Climate type "BSk" (Köppen classification)
                        if (!has(lineIn, "not shown")) {
                            isKoppen = true;
                            if (lineIn[18] == '"') { // two character classification
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Köppen Classification", lineIn.substr(16, 2));
                            } else {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Köppen Classification", lineIn.substr(16, 3));
                            }
                        } else {
                            isKoppen = false;
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Köppen Recommendation", lineIn.substr(2));
                        }
                    } else if (SELECT_CASE_var ==
                               StatLineType::KoppenDes1Line) { // - Tropical monsoonal or tradewind-coastal (short dry season, lat. 5-25°)
                        if (isKoppen) {
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Köppen Description", lineIn.substr(2));
                        }
                    } else if (SELECT_CASE_var ==
                               StatLineType::KoppenDes2Line) { // - Unbearably humid periods in summer, but passive cooling is possible
                        if (isKoppen) {
                            if (len(lineIn) > 3) {                 // avoid blank lines
                                if (lineIn.substr(2, 2) != "**") { // avoid line with warning
                                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Köppen Recommendation", lineIn.substr(2));
                                } else {
                                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Köppen Recommendation", "");
                                }
                            } else {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "Köppen Recommendation", "");
                            }
                        }
                    } else if ((SELECT_CASE_var == StatLineType::AshStdLine) || (SELECT_CASE_var == StatLineType::AshStdDes1Line) ||
                               (SELECT_CASE_var == StatLineType::AshStdDes2Line) || (SELECT_CASE_var == StatLineType::AshStdDes3Line)) {
                        //  - Climate type "1A" (ASHRAE Standards 90.1-2004 and 90.2-2004 Climate Zone)**
                        if (has(lineIn, "Standard")) {
                            ashZone = lineIn.substr(16, 2);
                            if (ashZone[1] == '"') ashZone[1] = ' ';
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Climate Zone", ashZone);
                            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedGenData, "Climate Zone", ashZone);
                            if (ashZone == "1A") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Very Hot-Humid");
                            } else if (ashZone == "1B") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Very Hot-Dry");
                            } else if (ashZone == "2A") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Hot-Humid");
                            } else if (ashZone == "2B") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Hot-Dry");
                            } else if (ashZone == "3A") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Warm-Humid");
                            } else if (ashZone == "3B") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Warm-Dry");
                            } else if (ashZone == "3C") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Warm-Marine");
                            } else if (ashZone == "4A") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Mixed-Humid");
                            } else if (ashZone == "4B") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Mixed-Dry");
                            } else if (ashZone == "4C") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Mixed-Marine");
                            } else if (ashZone == "5A") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Cool-Humid");
                            } else if (ashZone == "5B") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Cool-Dry");
                            } else if (ashZone == "5C") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Cool-Marine");
                            } else if (ashZone == "6A") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Cold-Humid");
                            } else if (ashZone == "6B") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Cold-Dry");
                            } else if (ashZone == "7 ") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Very Cold");
                            } else if (ashZone == "8 ") {
                                PreDefTableEntry(state, state.dataOutRptPredefined->pdchWthrVal, "ASHRAE Description", "Subarctic");
                            }
                        }
                    }
                }
                lineIn = "";
                lineTypeinterim = StatLineType::Initialized;
                if (lineType == StatLineType::AshStdDes3Line) lineTypeinterim = StatLineType::Initialized;
                if (lineType == StatLineType::AshStdDes2Line) lineTypeinterim = StatLineType::AshStdDes2Line;
                if (lineType == StatLineType::AshStdDes1Line) lineTypeinterim = StatLineType::AshStdDes1Line;
                if (lineType == StatLineType::AshStdLine) lineTypeinterim = StatLineType::AshStdLine;
                if (lineType == StatLineType::KoppenDes2Line) lineTypeinterim = StatLineType::Initialized;
                if (lineType == StatLineType::KoppenDes1Line) lineTypeinterim = StatLineType::KoppenDes1Line;
                if (lineType == StatLineType::KoppenLine) lineTypeinterim = StatLineType::KoppenLine;
            }
        }
    }

    std::string GetColumnUsingTabs(std::string const &inString, // Input String
                                   int const colNum             // Column number
    )
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   March 2008
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Assumes that the input string contains tabs that mark the
        //   separation between columns. Returns the string that appears
        //   in the column specified.

        // SUBROUTINE PARAMETER DEFINITIONS:
        static char const tb('\t'); // tab character

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        std::string::size_type startPos = 0;

        auto endPos = inString.find_first_of(tb);
        if (colNum == 1) {
            if (endPos == std::string::npos) return inString;
            return inString.substr(startPos, endPos - startPos);
        }
        if (endPos == std::string::npos) return "";

        int numCols = 1;
        while (numCols < colNum) {
            startPos = endPos + 1;
            endPos = inString.find_first_of(tb, startPos);
            ++numCols;
            if (endPos == std::string::npos) break;
        }
        if (colNum > numCols) return "";
        if (endPos == std::string::npos) endPos = inString.size();
        return inString.substr(startPos, endPos - startPos);
    }

    void FillRemainingPredefinedEntries(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   May 2007
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Just before writing the output reports, will gather up
        //   any additional report entries for the predefined reports.

        // Using/Aliasing
        using DataHeatBalance::BuildingPreDefRep;
        using DataHeatBalance::Lights;
        using DataHeatBalance::TotLights;
        using DataHeatBalance::ZnAirRpt;
        using DataHeatBalance::Zone;
        using DataHeatBalance::ZonePreDefRep;
        using DataHVACGlobals::NumPrimaryAirSys;
        using DataOutputs::iNumberOfAutoCalcedFields;
        using DataOutputs::iNumberOfAutoSizedFields;
        using DataOutputs::iNumberOfDefaultedFields;
        using DataOutputs::iNumberOfRecords;
        using DataOutputs::iTotalAutoCalculatableFields;
        using DataOutputs::iTotalAutoSizableFields;
        using DataOutputs::iTotalFieldsWithDefaults;

        using ScheduleManager::GetScheduleName;
        using ScheduleManager::ScheduleAverageHoursPerWeek;

        int iLight;
        int zonePt;
        int iZone;
        Real64 mult; // zone list and group multipliers
        static Real64 totalVolume(0.0);
        static int numUncondZones(0);
        static int numCondZones(0);
        int StartOfWeek;
        static Real64 HrsPerWeek(0.0);
        Real64 consumptionTotal;
        // sensible heat gain report totals
        static Real64 totalZoneEqHt(0.0);
        static Real64 totalZoneEqCl(0.0);
        static Real64 totalHvacATUHt(0.0);
        static Real64 totalHvacATUCl(0.0);
        static Real64 totalSurfHt(0.0);
        static Real64 totalSurfCl(0.0);
        static Real64 totalPeoplAdd(0.0);
        static Real64 totalLiteAdd(0.0);
        static Real64 totalEquipAdd(0.0);
        static Real64 totalWindAdd(0.0);
        static Real64 totalIzaAdd(0.0);
        static Real64 totalInfilAdd(0.0);
        static Real64 totalOtherAdd(0.0);
        static Real64 totalEquipRem(0.0);
        static Real64 totalWindRem(0.0);
        static Real64 totalIzaRem(0.0);
        static Real64 totalInfilRem(0.0);
        static Real64 totalOtherRem(0.0);

        auto &ort(state.dataOutRptTab);

        StartOfWeek = state.dataEnvrn->RunPeriodStartDayOfWeek;
        if (StartOfWeek == 0) StartOfWeek = 2; // if the first day of the week has not been set yet, assume monday

        // Interior Connected Lighting Power
        consumptionTotal = 0.0;
        for (iLight = 1; iLight <= TotLights; ++iLight) {
            zonePt = Lights(iLight).ZonePtr;
            mult = Zone(zonePt).Multiplier * Zone(zonePt).ListMultiplier;
            if (Zone(zonePt).SystemZoneNodeNumber > 0) { // conditioned y/n
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchInLtCond, Lights(iLight).Name, "Y");
            } else {
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchInLtCond, Lights(iLight).Name, "N");
            }
            PreDefTableEntry(state,
                state.dataOutRptPredefined->pdchInLtAvgHrSchd, Lights(iLight).Name, ScheduleAverageHoursPerWeek(state, Lights(iLight).SchedPtr, StartOfWeek, state.dataEnvrn->CurrentYearIsLeapYear));
            // average operating hours per week
            if (ort->gatherElapsedTimeBEPS > 0) {
                HrsPerWeek = 24 * 7 * Lights(iLight).SumTimeNotZeroCons / ort->gatherElapsedTimeBEPS;
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchInLtAvgHrOper, Lights(iLight).Name, HrsPerWeek);
            }
            // full load hours per week
            if ((Lights(iLight).DesignLevel * ort->gatherElapsedTimeBEPS) > 0) {
                HrsPerWeek = 24 * 7 * Lights(iLight).SumConsumption / (Lights(iLight).DesignLevel * ort->gatherElapsedTimeBEPS * DataGlobalConstants::SecInHour);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchInLtFullLoadHrs, Lights(iLight).Name, HrsPerWeek);
            }
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchInLtConsump, Lights(iLight).Name, Lights(iLight).SumConsumption * mult / 1000000000.0);
            consumptionTotal += Lights(iLight).SumConsumption / 1000000000.0;
        }
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchInLtConsump, "Interior Lighting Total", consumptionTotal);

        // Exterior Lighting
        consumptionTotal = 0.0;
        for (iLight = 1; iLight <= state.dataExteriorEnergyUse->NumExteriorLights; ++iLight) {
            if (state.dataExteriorEnergyUse->ExteriorLights(iLight).ControlMode == ExteriorEnergyUse::LightControlType::ScheduleOnly) { // photocell/schedule
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchExLtAvgHrSchd,
                                 state.dataExteriorEnergyUse->ExteriorLights(iLight).Name,
                                 ScheduleAverageHoursPerWeek(state, state.dataExteriorEnergyUse->ExteriorLights(iLight).SchedPtr, StartOfWeek, state.dataEnvrn->CurrentYearIsLeapYear));
            }
            // average operating hours per week
            if (ort->gatherElapsedTimeBEPS > 0) {
                HrsPerWeek = 24 * 7 * state.dataExteriorEnergyUse->ExteriorLights(iLight).SumTimeNotZeroCons / ort->gatherElapsedTimeBEPS;
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchExLtAvgHrOper, state.dataExteriorEnergyUse->ExteriorLights(iLight).Name, HrsPerWeek);
            }
            // full load hours per week
            if ((state.dataExteriorEnergyUse->ExteriorLights(iLight).DesignLevel * ort->gatherElapsedTimeBEPS) > 0) {
                HrsPerWeek =
                    24 * 7 * state.dataExteriorEnergyUse->ExteriorLights(iLight).SumConsumption / (state.dataExteriorEnergyUse->ExteriorLights(iLight).DesignLevel * ort->gatherElapsedTimeBEPS * DataGlobalConstants::SecInHour);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchExLtFullLoadHrs, state.dataExteriorEnergyUse->ExteriorLights(iLight).Name, HrsPerWeek);
            }
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchExLtConsump, state.dataExteriorEnergyUse->ExteriorLights(iLight).Name, state.dataExteriorEnergyUse->ExteriorLights(iLight).SumConsumption / 1000000000.0);
            consumptionTotal += state.dataExteriorEnergyUse->ExteriorLights(iLight).SumConsumption / 1000000000.0;
        }
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchExLtConsump, "Exterior Lighting Total", consumptionTotal);

        // outside air ventilation
        for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
            if (Zone(iZone).SystemZoneNodeNumber >= 0) { // conditioned zones only
                if (Zone(iZone).isNominalOccupied) {
                    // occupants
                    if (ZonePreDefRep(iZone).NumOccAccumTime > 0) {
                        PreDefTableEntry(state,
                            state.dataOutRptPredefined->pdchOaoAvgNumOcc1, Zone(iZone).Name, ZonePreDefRep(iZone).NumOccAccum / ZonePreDefRep(iZone).NumOccAccumTime);
                        PreDefTableEntry(state,
                            state.dataOutRptPredefined->pdchOaoAvgNumOcc2, Zone(iZone).Name, ZonePreDefRep(iZone).NumOccAccum / ZonePreDefRep(iZone).NumOccAccumTime);
                    }
                    // Mechanical ventilation
                    if (Zone(iZone).Volume > 0 && ZonePreDefRep(iZone).TotTimeOcc > 0) {
                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchOaoAvgMechVent,
                                         Zone(iZone).Name,
                                         ZonePreDefRep(iZone).MechVentVolTotal / (ZonePreDefRep(iZone).TotTimeOcc * Zone(iZone).Volume *
                                                                                  Zone(iZone).Multiplier * Zone(iZone).ListMultiplier),
                                         3);
                    }
                    if ((Zone(iZone).Volume > 0) && (ZonePreDefRep(iZone).TotTimeOcc > 0)) {
                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchOaoMinMechVent,
                                         Zone(iZone).Name,
                                         ZonePreDefRep(iZone).MechVentVolMin /
                                             (Zone(iZone).Volume * Zone(iZone).Multiplier * Zone(iZone).ListMultiplier),
                                         3);
                    }
                    // infiltration
                    if (Zone(iZone).Volume > 0 && ZonePreDefRep(iZone).TotTimeOcc > 0) {
                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchOaoAvgInfil,
                                         Zone(iZone).Name,
                                         ZonePreDefRep(iZone).InfilVolTotal / (ZonePreDefRep(iZone).TotTimeOcc * Zone(iZone).Volume),
                                         3);
                    }
                    if ((Zone(iZone).Volume > 0) && (ZonePreDefRep(iZone).TotTimeOcc > 0)) {
                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchOaoMinInfil, Zone(iZone).Name, ZonePreDefRep(iZone).InfilVolMin / (Zone(iZone).Volume), 3);
                    }
                    // AFN infiltration -- check that afn sim is being done.
                    if (AirflowNetwork::SimulateAirflowNetwork < AirflowNetwork::AirflowNetworkControlMultizone) {
                        ZonePreDefRep(iZone).AFNInfilVolMin = 0.0;
                        ZonePreDefRep(iZone).AFNInfilVolTotal = 0.0;
                        if (!(AirflowNetwork::SimulateAirflowNetwork == AirflowNetwork::AirflowNetworkControlMultizone ||
                              AirflowNetwork::SimulateAirflowNetwork == AirflowNetwork::AirflowNetworkControlMultiADS)) {
                            ZonePreDefRep(iZone).AFNInfilVolMin = 0.0;
                            ZonePreDefRep(iZone).AFNInfilVolTotal = 0.0;
                        }
                    }
                    if (Zone(iZone).Volume > 0 && ZonePreDefRep(iZone).TotTimeOcc > 0) {
                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchOaoAvgAFNInfil,
                                         Zone(iZone).Name,
                                         ZonePreDefRep(iZone).AFNInfilVolTotal / (ZonePreDefRep(iZone).TotTimeOcc * Zone(iZone).Volume),
                                         3);
                    }
                    if ((Zone(iZone).Volume > 0) && (ZonePreDefRep(iZone).TotTimeOcc > 0)) {
                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchOaoMinAFNInfil, Zone(iZone).Name, ZonePreDefRep(iZone).AFNInfilVolMin / (Zone(iZone).Volume), 3);
                    }
                    // simple 'ZoneVentilation'
                    if (Zone(iZone).Volume > 0 && ZonePreDefRep(iZone).TotTimeOcc > 0) {
                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchOaoAvgSimpVent,
                                         Zone(iZone).Name,
                                         ZonePreDefRep(iZone).SimpVentVolTotal / (ZonePreDefRep(iZone).TotTimeOcc * Zone(iZone).Volume),
                                         3);
                    }
                    if ((Zone(iZone).Volume > 0) && (ZonePreDefRep(iZone).TotTimeOcc > 0)) {
                        PreDefTableEntry(state, state.dataOutRptPredefined->pdchOaoMinSimpVent, Zone(iZone).Name, ZonePreDefRep(iZone).SimpVentVolMin / (Zone(iZone).Volume), 3);
                    }

                    // Zone volume
                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchOaoZoneVol1, Zone(iZone).Name, Zone(iZone).Volume);
                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchOaoZoneVol2, Zone(iZone).Name, Zone(iZone).Volume);
                    totalVolume += Zone(iZone).Volume;
                }
            }
        }

        // Add the number of central air distributions system to the count report
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchHVACcntVal, "HVAC Air Loops", NumPrimaryAirSys);
        // Add the number of conditioned and unconditioned zones to the count report
        for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
            if (Zone(iZone).SystemZoneNodeNumber > 0) { // conditioned zones only
                ++numCondZones;
            } else {
                ++numUncondZones;
            }
        }
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchHVACcntVal, "Conditioned Zones", numCondZones);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchHVACcntVal, "Unconditioned Zones", numUncondZones);
        // add the number of plenums to the count report
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchHVACcntVal, "Supply Plenums", state.dataZonePlenum->NumZoneSupplyPlenums);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchHVACcntVal, "Return Plenums", state.dataZonePlenum->NumZoneReturnPlenums);

        // Started to create a total row but did not fully implement
        // CALL PreDefTableEntry(state, pdchOaoZoneVol1,'Total OA Avg', totalVolume)
        // CALL PreDefTableEntry(state, pdchOaoZoneVol2,'Total OA Min', totalVolume)

        // Add footnote saying if it is a design day or other kind of environment

        // Field counts
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchFieldCntVal, "IDF Objects", iNumberOfRecords);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchFieldCntVal, "Defaulted Fields", iNumberOfDefaultedFields);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchFieldCntVal, "Fields with Defaults", iTotalFieldsWithDefaults);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchFieldCntVal, "Autosized Fields", iNumberOfAutoSizedFields);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchFieldCntVal, "Autosizable Fields", iTotalAutoSizableFields);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchFieldCntVal, "Autocalculated Fields", iNumberOfAutoCalcedFields);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchFieldCntVal, "Autocalculatable Fields", iTotalAutoCalculatableFields);

        for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
            // annual
            // PreDefTableEntry(state,  pdchSHGSAnHvacHt, Zone( iZone ).Name, ZonePreDefRep( iZone ).SHGSAnHvacHt * convertJtoGJ, 3 );
            // PreDefTableEntry(state,  pdchSHGSAnHvacCl, Zone( iZone ).Name, ZonePreDefRep( iZone ).SHGSAnHvacCl * convertJtoGJ, 3 );
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnZoneEqHt, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnZoneEqHt * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnZoneEqCl, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnZoneEqCl * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnHvacATUHt, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnHvacATUHt * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnHvacATUCl, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnHvacATUCl * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnSurfHt, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnSurfHt * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnSurfCl, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnSurfCl * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnPeoplAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnPeoplAdd * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnLiteAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnLiteAdd * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnEquipAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnEquipAdd * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnWindAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnWindAdd * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnIzaAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnIzaAdd * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnInfilAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnInfilAdd * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnOtherAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnOtherAdd * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnEquipRem, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnEquipRem * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnWindRem, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnWindRem * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnIzaRem, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnIzaRem * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnInfilRem, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnInfilRem * DataGlobalConstants::convertJtoGJ, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnOtherRem, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSAnOtherRem * DataGlobalConstants::convertJtoGJ, 3);
            // peak cooling
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClTimePeak, Zone(iZone).Name, DateToString(ZonePreDefRep(iZone).clPtTimeStamp));
            // PreDefTableEntry(state,  pdchSHGSClHvacHt, Zone( iZone ).Name, ZonePreDefRep( iZone ).SHGSClHvacHt );
            // PreDefTableEntry(state,  pdchSHGSClHvacCl, Zone( iZone ).Name, ZonePreDefRep( iZone ).SHGSClHvacCl );
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClHvacHt, Zone(iZone).Name, (ZonePreDefRep(iZone).SHGSClHvacHt - ZonePreDefRep(iZone).SHGSClHvacATUHt));
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClHvacCl, Zone(iZone).Name, (ZonePreDefRep(iZone).SHGSClHvacCl - ZonePreDefRep(iZone).SHGSClHvacATUCl));
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClHvacATUHt, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClHvacATUHt);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClHvacATUCl, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClHvacATUCl);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClSurfHt, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClSurfHt);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClSurfCl, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClSurfCl);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClPeoplAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClPeoplAdd);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClLiteAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClLiteAdd);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClEquipAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClEquipAdd);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClWindAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClWindAdd);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClIzaAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClIzaAdd);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClInfilAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClInfilAdd);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClOtherAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClOtherAdd);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClEquipRem, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClEquipRem);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClWindRem, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClWindRem);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClIzaRem, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClIzaRem);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClInfilRem, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClInfilRem);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClOtherRem, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSClOtherRem);
            // peak heating
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtTimePeak, Zone(iZone).Name, DateToString(ZonePreDefRep(iZone).htPtTimeStamp));
            // PreDefTableEntry(state,  pdchSHGSHtHvacHt, Zone( iZone ).Name, ZonePreDefRep( iZone ).SHGSHtHvacHt );
            // PreDefTableEntry(state,  pdchSHGSHtHvacCl, Zone( iZone ).Name, ZonePreDefRep( iZone ).SHGSHtHvacCl );
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtHvacHt, Zone(iZone).Name, (ZonePreDefRep(iZone).SHGSHtHvacHt - ZonePreDefRep(iZone).SHGSHtHvacATUHt));
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtHvacCl, Zone(iZone).Name, (ZonePreDefRep(iZone).SHGSHtHvacCl - ZonePreDefRep(iZone).SHGSHtHvacATUCl));
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtHvacATUHt, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtHvacATUHt);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtHvacATUCl, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtHvacATUCl);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtSurfHt, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtSurfHt);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtSurfCl, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtSurfCl);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtPeoplAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtPeoplAdd);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtLiteAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtLiteAdd);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtEquipAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtEquipAdd);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtWindAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtWindAdd);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtIzaAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtIzaAdd);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtInfilAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtInfilAdd);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtOtherAdd, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtOtherAdd);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtEquipRem, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtEquipRem);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtWindRem, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtWindRem);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtIzaRem, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtIzaRem);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtInfilRem, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtInfilRem);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtOtherRem, Zone(iZone).Name, ZonePreDefRep(iZone).SHGSHtOtherRem);
        }
        // totals for annual report
        for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
            totalZoneEqHt += ZonePreDefRep(iZone).SHGSAnZoneEqHt;
            totalZoneEqCl += ZonePreDefRep(iZone).SHGSAnZoneEqCl;
            totalHvacATUHt += ZonePreDefRep(iZone).SHGSAnHvacATUHt;
            totalHvacATUCl += ZonePreDefRep(iZone).SHGSAnHvacATUCl;
            totalSurfHt += ZonePreDefRep(iZone).SHGSAnSurfHt;
            totalSurfCl += ZonePreDefRep(iZone).SHGSAnSurfCl;
            totalPeoplAdd += ZonePreDefRep(iZone).SHGSAnPeoplAdd;
            totalLiteAdd += ZonePreDefRep(iZone).SHGSAnLiteAdd;
            totalEquipAdd += ZonePreDefRep(iZone).SHGSAnEquipAdd;
            totalWindAdd += ZonePreDefRep(iZone).SHGSAnWindAdd;
            totalIzaAdd += ZonePreDefRep(iZone).SHGSAnIzaAdd;
            totalInfilAdd += ZonePreDefRep(iZone).SHGSAnInfilAdd;
            totalOtherAdd += ZonePreDefRep(iZone).SHGSAnOtherAdd;
            totalEquipRem += ZonePreDefRep(iZone).SHGSAnEquipRem;
            totalWindRem += ZonePreDefRep(iZone).SHGSAnWindRem;
            totalIzaRem += ZonePreDefRep(iZone).SHGSAnIzaRem;
            totalInfilRem += ZonePreDefRep(iZone).SHGSAnInfilRem;
            totalOtherRem += ZonePreDefRep(iZone).SHGSAnOtherRem;
        }
        // PreDefTableEntry(state,  pdchSHGSAnHvacHt, "Total Facility", totalHvacHt * convertJtoGJ, 3 );
        // PreDefTableEntry(state,  pdchSHGSAnHvacCl, "Total Facility", totalHvacCl * convertJtoGJ, 3 );
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnZoneEqHt, "Total Facility", totalZoneEqHt * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnZoneEqCl, "Total Facility", totalZoneEqCl * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnHvacATUHt, "Total Facility", totalHvacATUHt * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnHvacATUCl, "Total Facility", totalHvacATUCl * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnSurfHt, "Total Facility", totalSurfHt * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnSurfCl, "Total Facility", totalSurfCl * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnPeoplAdd, "Total Facility", totalPeoplAdd * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnLiteAdd, "Total Facility", totalLiteAdd * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnEquipAdd, "Total Facility", totalEquipAdd * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnWindAdd, "Total Facility", totalWindAdd * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnIzaAdd, "Total Facility", totalIzaAdd * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnInfilAdd, "Total Facility", totalInfilAdd * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnOtherAdd, "Total Facility", totalOtherAdd * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnEquipRem, "Total Facility", totalEquipRem * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnWindRem, "Total Facility", totalWindRem * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnIzaRem, "Total Facility", totalIzaRem * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnInfilRem, "Total Facility", totalInfilRem * DataGlobalConstants::convertJtoGJ, 3);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSAnOtherRem, "Total Facility", totalOtherRem * DataGlobalConstants::convertJtoGJ, 3);
        // building level results for peak cooling
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClTimePeak, "Total Facility", DateToString(BuildingPreDefRep.clPtTimeStamp));
        // PreDefTableEntry(state,  pdchSHGSClHvacHt, "Total Facility", BuildingPreDefRep.SHGSClHvacHt );
        // PreDefTableEntry(state,  pdchSHGSClHvacCl, "Total Facility", BuildingPreDefRep.SHGSClHvacCl );
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClHvacHt, "Total Facility", (BuildingPreDefRep.SHGSClHvacHt - BuildingPreDefRep.SHGSClHvacATUHt));
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClHvacCl, "Total Facility", (BuildingPreDefRep.SHGSClHvacCl - BuildingPreDefRep.SHGSClHvacATUCl));
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClHvacATUHt, "Total Facility", BuildingPreDefRep.SHGSClHvacATUHt);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClHvacATUCl, "Total Facility", BuildingPreDefRep.SHGSClHvacATUCl);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClSurfHt, "Total Facility", BuildingPreDefRep.SHGSClSurfHt);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClSurfCl, "Total Facility", BuildingPreDefRep.SHGSClSurfCl);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClPeoplAdd, "Total Facility", BuildingPreDefRep.SHGSClPeoplAdd);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClLiteAdd, "Total Facility", BuildingPreDefRep.SHGSClLiteAdd);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClEquipAdd, "Total Facility", BuildingPreDefRep.SHGSClEquipAdd);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClWindAdd, "Total Facility", BuildingPreDefRep.SHGSClWindAdd);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClIzaAdd, "Total Facility", BuildingPreDefRep.SHGSClIzaAdd);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClInfilAdd, "Total Facility", BuildingPreDefRep.SHGSClInfilAdd);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClOtherAdd, "Total Facility", BuildingPreDefRep.SHGSClOtherAdd);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClEquipRem, "Total Facility", BuildingPreDefRep.SHGSClEquipRem);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClWindRem, "Total Facility", BuildingPreDefRep.SHGSClWindRem);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClIzaRem, "Total Facility", BuildingPreDefRep.SHGSClIzaRem);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClInfilRem, "Total Facility", BuildingPreDefRep.SHGSClInfilRem);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSClOtherRem, "Total Facility", BuildingPreDefRep.SHGSClOtherRem);
        // building level results for peak heating
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtTimePeak, "Total Facility", DateToString(BuildingPreDefRep.htPtTimeStamp));
        // PreDefTableEntry(state,  pdchSHGSHtHvacHt, "Total Facility", BuildingPreDefRep.SHGSHtHvacHt );
        // PreDefTableEntry(state,  pdchSHGSHtHvacCl, "Total Facility", BuildingPreDefRep.SHGSHtHvacCl );
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtHvacHt, "Total Facility", (BuildingPreDefRep.SHGSHtHvacHt - BuildingPreDefRep.SHGSHtHvacATUHt));
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtHvacCl, "Total Facility", (BuildingPreDefRep.SHGSHtHvacCl - BuildingPreDefRep.SHGSHtHvacATUCl));
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtHvacATUHt, "Total Facility", BuildingPreDefRep.SHGSHtHvacATUHt);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtHvacATUCl, "Total Facility", BuildingPreDefRep.SHGSHtHvacATUCl);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtSurfHt, "Total Facility", BuildingPreDefRep.SHGSHtSurfHt);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtSurfCl, "Total Facility", BuildingPreDefRep.SHGSHtSurfCl);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtPeoplAdd, "Total Facility", BuildingPreDefRep.SHGSHtPeoplAdd);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtLiteAdd, "Total Facility", BuildingPreDefRep.SHGSHtLiteAdd);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtEquipAdd, "Total Facility", BuildingPreDefRep.SHGSHtEquipAdd);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtWindAdd, "Total Facility", BuildingPreDefRep.SHGSHtWindAdd);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtIzaAdd, "Total Facility", BuildingPreDefRep.SHGSHtIzaAdd);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtInfilAdd, "Total Facility", BuildingPreDefRep.SHGSHtInfilAdd);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtOtherAdd, "Total Facility", BuildingPreDefRep.SHGSHtOtherAdd);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtEquipRem, "Total Facility", BuildingPreDefRep.SHGSHtEquipRem);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtWindRem, "Total Facility", BuildingPreDefRep.SHGSHtWindRem);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtIzaRem, "Total Facility", BuildingPreDefRep.SHGSHtIzaRem);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtInfilRem, "Total Facility", BuildingPreDefRep.SHGSHtInfilRem);
        PreDefTableEntry(state, state.dataOutRptPredefined->pdchSHGSHtOtherRem, "Total Facility", BuildingPreDefRep.SHGSHtOtherRem);

        // LEED Report
        // 1.1A-General Information
        // CALL PreDefTableEntry(state, pdchLeedGenData,'Principal Heating Source','-')
        if (state.dataEnvrn->EnvironmentName == state.dataEnvrn->WeatherFileLocationTitle) {
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedGenData, "Weather File", state.dataEnvrn->EnvironmentName);
        } else {
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedGenData, "Weather File", state.dataEnvrn->EnvironmentName + " ** " + state.dataEnvrn->WeatherFileLocationTitle);
        }

        // CALL PreDefTableEntry(state, pdchLeedGenData,'Climate Zone','-')
        // CALL PreDefTableEntry(state, pdchLeedGenData,'Heating Degree Days','-')
        // CALL PreDefTableEntry(state, pdchLeedGenData,'Cooling Degree Days','-')
        if (ort->unitsStyle == iUnitsStyle::InchPound) {
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedGenData, "Total gross floor area [ft2]", "-");
        } else {
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedGenData, "Total gross floor area [m2]", "-");
        }
        // LEED schedule sub table
        for (long iSch = 1; iSch <= ScheduleManager::NumSchedules; ++iSch) {
            std::string curSchName = ScheduleManager::Schedule(iSch).Name;
            std::string curSchType = ScheduleManager::GetScheduleType(state, iSch);
            if (UtilityRoutines::SameString(curSchType, "FRACTION")) {
                PreDefTableEntry(state,
                    state.dataOutRptPredefined->pdchLeedEflhEflh, curSchName, ScheduleManager::ScheduleAnnualFullLoadHours(state, iSch, StartOfWeek, state.dataEnvrn->CurrentYearIsLeapYear), 0);
                PreDefTableEntry(state,
                    state.dataOutRptPredefined->pdchLeedEflhNonZerHrs, curSchName, ScheduleManager::ScheduleHoursGT1perc(state, iSch, StartOfWeek, state.dataEnvrn->CurrentYearIsLeapYear), 0);
            }
        }
        // fill the LEED setpoint table
        ZoneTempPredictorCorrector::FillPredefinedTableOnThermostatSetpoints(state);
    }

    void WriteMonthlyTables(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   August 2003
        //       MODIFIED       January 2010, Kyle Benne
        //                      Added SQLite output
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Set up the monthly tabular report results

        // METHODOLOGY EMPLOYED:
        //   Creates several arrays that are passed to the WriteTable
        //   routine.  All arrays are strings so numbers need to be
        //   converted prior to calling WriteTable.

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        Array1D_string columnHead;
        Array1D_int columnWidth;
        Array1D_string rowHead(16);
        Array2D_string tableBody;
        std::string curAggString;
        int iInput;
        int jTable;
        int kColumn;
        int lMonth;
        int curTable;
        int curCol;
        Real64 curVal;
        Real64 curConversionFactor;
        static Real64 curConversionOffset(0.0);
        int columnUsedCount;
        int columnRecount;
        int digitsShown;
        Real64 minVal;
        Real64 maxVal;
        Real64 sumVal;
        Real64 sumDuration;
        std::string curUnits;
        std::string energyUnitsString;
        Real64 energyUnitsConversionFactor;
        int indexUnitConv;
        std::string varNameWithUnits;
        Real64 veryLarge;
        Real64 verySmall;

        static Real64 const storedMaxVal(std::numeric_limits<Real64>::max());
        static Real64 const storedMinVal(std::numeric_limits<Real64>::lowest());

        rowHead(1) = "January";
        rowHead(2) = "February";
        rowHead(3) = "March";
        rowHead(4) = "April";
        rowHead(5) = "May";
        rowHead(6) = "June";
        rowHead(7) = "July";
        rowHead(8) = "August";
        rowHead(9) = "September";
        rowHead(10) = "October";
        rowHead(11) = "November";
        rowHead(12) = "December";
        rowHead(13) = "";
        rowHead(14) = "Annual Sum or Average";
        rowHead(15) = "Minimum of Months";
        rowHead(16) = "Maximum of Months";

        std::unordered_map<iAggType, std::string> aggString = {
            {iAggType::SumOrAvg, ""},
            {iAggType::Maximum, " Maximum "},
            {iAggType::Minimum, " MINIMUM "},
            {iAggType::ValueWhenMaxMin, " AT MAX/MIN "},
            {iAggType::HoursZero, " HOURS ZERO "},
            {iAggType::HoursNonZero, " HOURS NON-ZERO "},
            {iAggType::HoursPositive, " HOURS POSITIVE "},
            {iAggType::HoursNonPositive, " HOURS NON-POSITIVE "},
            {iAggType::HoursNegative, " HOURS NEGATIVE "},
            {iAggType::HoursNonNegative, " HOURS NON-NEGATIVE "},
            {iAggType::SumOrAverageHoursShown, " FOR HOURS SHOWN "},
            {iAggType::MaximumDuringHoursShown, " MAX FOR HOURS SHOWN "},
            {iAggType::MinimumDuringHoursShown, " MIN FOR HOURS SHOWN "},
        };

        veryLarge = 1.0E280;
        verySmall = -1.0E280;

        auto &ort(state.dataOutRptTab);

        // set the unit conversion
        if (ort->unitsStyle == iUnitsStyle::None) {
            energyUnitsString = "J";
            energyUnitsConversionFactor = 1.0;
        } else if (ort->unitsStyle == iUnitsStyle::JtoKWH) {
            energyUnitsString = "kWh";
            energyUnitsConversionFactor = 1.0 / 3600000.0;
        } else if (ort->unitsStyle == iUnitsStyle::JtoMJ) {
            energyUnitsString = "MJ";
            energyUnitsConversionFactor = 1.0 / 1000000.0;
        } else if (ort->unitsStyle == iUnitsStyle::JtoGJ) {
            energyUnitsString = "GJ";
            energyUnitsConversionFactor = 1.0 / 1000000000.0;
        } else { // Should never happen but assures compilers of initialization
            energyUnitsString = "J";
            energyUnitsConversionFactor = 1.0;
        }

        // loop through each input to get the name of the tables
        for (iInput = 1; iInput <= ort->MonthlyInputCount; ++iInput) {
            // loop through each report and
            digitsShown = ort->MonthlyInput(iInput).showDigits;
            for (jTable = 1; jTable <= ort->MonthlyInput(iInput).numTables; ++jTable) {
                curTable = jTable + ort->MonthlyInput(iInput).firstTable - 1;
                // first loop through and count how many 'columns' are defined
                // since max and min actually define two columns (the value
                // and the timestamp).
                columnUsedCount = 0;
                for (kColumn = 1; kColumn <= ort->MonthlyTables(curTable).numColumns; ++kColumn) {
                    curCol = kColumn + ort->MonthlyTables(curTable).firstColumn - 1;
                    {
                        auto const SELECT_CASE_var(ort->MonthlyColumns(curCol).aggType);
                        if ((SELECT_CASE_var == iAggType::SumOrAvg) || (SELECT_CASE_var == iAggType::ValueWhenMaxMin) ||
                            (SELECT_CASE_var == iAggType::HoursZero) || (SELECT_CASE_var == iAggType::HoursNonZero) ||
                            (SELECT_CASE_var == iAggType::HoursPositive) || (SELECT_CASE_var == iAggType::HoursNonPositive) ||
                            (SELECT_CASE_var == iAggType::HoursNegative) || (SELECT_CASE_var == iAggType::HoursNonNegative) ||
                            (SELECT_CASE_var == iAggType::SumOrAverageHoursShown)) {
                            ++columnUsedCount;
                        } else if ((SELECT_CASE_var == iAggType::Maximum) || (SELECT_CASE_var == iAggType::Minimum) ||
                                   (SELECT_CASE_var == iAggType::MaximumDuringHoursShown) || (SELECT_CASE_var == iAggType::MinimumDuringHoursShown)) {
                            columnUsedCount += 2;
                        }
                    }
                } // jColumn
                columnHead.allocate(columnUsedCount);
                columnWidth.dimension(columnUsedCount, 14); // array assignment - same for all columns
                tableBody.allocate(columnUsedCount, 16);
                tableBody = ""; // set entire table to blank as default
                columnRecount = 0;
                for (kColumn = 1; kColumn <= ort->MonthlyTables(curTable).numColumns; ++kColumn) {
                    curCol = kColumn + ort->MonthlyTables(curTable).firstColumn - 1;
                    curAggString = aggString.at(ort->MonthlyColumns(curCol).aggType);
                    if (len(curAggString) > 0) {
                        curAggString = " {" + stripped(curAggString) + '}';
                    }
                    // do the unit conversions
                    if (ort->unitsStyle == iUnitsStyle::InchPound) {
                        varNameWithUnits = ort->MonthlyColumns(curCol).varName + unitEnumToStringBrackets(ort->MonthlyColumns(curCol).units);
                        LookupSItoIP(state, varNameWithUnits, indexUnitConv, curUnits);
                        GetUnitConversion(state, indexUnitConv, curConversionFactor, curConversionOffset, curUnits);
                    } else { // just do the Joule conversion
                        // if units is in Joules, convert if specified
                        if (UtilityRoutines::SameString(unitEnumToString(ort->MonthlyColumns(curCol).units), "J")) {
                            curUnits = energyUnitsString;
                            curConversionFactor = energyUnitsConversionFactor;
                            curConversionOffset = 0.0;
                        } else { // if not joules don't perform conversion
                            curUnits = unitEnumToString(ort->MonthlyColumns(curCol).units);
                            curConversionFactor = 1.0;
                            curConversionOffset = 0.0;
                        }
                    }
                    {
                        auto const SELECT_CASE_var(ort->MonthlyColumns(curCol).aggType);
                        if ((SELECT_CASE_var == iAggType::SumOrAvg) || (SELECT_CASE_var == iAggType::SumOrAverageHoursShown)) {
                            ++columnRecount;
                            // put in the name of the variable for the column
                            columnHead(columnRecount) = ort->MonthlyColumns(curCol).varName + curAggString + " [" + curUnits + ']';
                            sumVal = 0.0;
                            sumDuration = 0.0;
                            minVal = storedMaxVal;
                            maxVal = storedMinVal;
                            for (lMonth = 1; lMonth <= 12; ++lMonth) {
                                if (ort->MonthlyColumns(curCol).avgSum ==
                                    OutputProcessor::StoreType::Averaged) { // if it is a average variable divide by duration
                                    if (ort->MonthlyColumns(curCol).duration(lMonth) != 0) {
                                        curVal =
                                            ((ort->MonthlyColumns(curCol).reslt(lMonth) / ort->MonthlyColumns(curCol).duration(lMonth)) * curConversionFactor) +
                                            curConversionOffset;
                                    } else {
                                        curVal = 0.0;
                                    }
                                    sumVal += (ort->MonthlyColumns(curCol).reslt(lMonth) * curConversionFactor) + curConversionOffset;
                                    sumDuration += ort->MonthlyColumns(curCol).duration(lMonth);
                                } else {
                                    curVal = (ort->MonthlyColumns(curCol).reslt(lMonth) * curConversionFactor) + curConversionOffset;
                                    sumVal += curVal;
                                }
                                if (ort->IsMonthGathered(lMonth)) {
                                    tableBody(columnRecount, lMonth) = RealToStr(curVal, digitsShown);
                                    if (curVal > maxVal) maxVal = curVal;
                                    if (curVal < minVal) minVal = curVal;
                                } else {
                                    tableBody(columnRecount, lMonth) = "-";
                                }
                            } // lMonth
                            // add the summary to bottom
                            if (ort->MonthlyColumns(curCol).avgSum ==
                                OutputProcessor::StoreType::Averaged) { // if it is a average variable divide by duration
                                if (sumDuration > 0) {
                                    tableBody(columnRecount, 14) = RealToStr(sumVal / sumDuration, digitsShown);
                                } else {
                                    tableBody(columnRecount, 14) = "";
                                }
                            } else {
                                tableBody(columnRecount, 14) = RealToStr(sumVal, digitsShown);
                            }
                            if (minVal != storedMaxVal) {
                                tableBody(columnRecount, 15) = RealToStr(minVal, digitsShown);
                            }
                            if (maxVal != storedMinVal) {
                                tableBody(columnRecount, 16) = RealToStr(maxVal, digitsShown);
                            }
                        } else if ((SELECT_CASE_var == iAggType::HoursZero) || (SELECT_CASE_var == iAggType::HoursNonZero) ||
                                   (SELECT_CASE_var == iAggType::HoursPositive) || (SELECT_CASE_var == iAggType::HoursNonPositive) ||
                                   (SELECT_CASE_var == iAggType::HoursNegative) || (SELECT_CASE_var == iAggType::HoursNonNegative)) {

                            ++columnRecount;
                            // put in the name of the variable for the column
                            columnHead(columnRecount) = ort->MonthlyColumns(curCol).varName + curAggString + " [HOURS]";
                            sumVal = 0.0;
                            minVal = storedMaxVal;
                            maxVal = storedMinVal;
                            for (lMonth = 1; lMonth <= 12; ++lMonth) {
                                curVal = ort->MonthlyColumns(curCol).reslt(lMonth);
                                if (ort->IsMonthGathered(lMonth)) {
                                    tableBody(columnRecount, lMonth) = RealToStr(curVal, digitsShown);
                                    sumVal += curVal;
                                    if (curVal > maxVal) maxVal = curVal;
                                    if (curVal < minVal) minVal = curVal;
                                } else {
                                    tableBody(columnRecount, lMonth) = "-";
                                }
                            } // lMonth
                            // add the summary to bottom
                            tableBody(columnRecount, 14) = RealToStr(sumVal, digitsShown);
                            if (minVal != storedMaxVal) {
                                tableBody(columnRecount, 15) = RealToStr(minVal, digitsShown);
                            }
                            if (maxVal != storedMinVal) {
                                tableBody(columnRecount, 16) = RealToStr(maxVal, digitsShown);
                            }
                        } else if (SELECT_CASE_var == iAggType::ValueWhenMaxMin) {
                            ++columnRecount;
                            if (ort->MonthlyColumns(curCol).avgSum == OutputProcessor::StoreType::Summed) {
                                curUnits += "/s";
                            }
                            if (UtilityRoutines::SameString(curUnits, "J/s")) {
                                curUnits = "W";
                            }
                            // CR7783 fix
                            if (UtilityRoutines::SameString(curUnits, "kWh/s")) {
                                curUnits = "W";
                                curConversionFactor *= 3600000.0;
                            }
                            if (UtilityRoutines::SameString(curUnits, "GJ/s")) {
                                curUnits = "kW";
                                curConversionFactor *= 1000000.0;
                            }
                            if (UtilityRoutines::SameString(curUnits, "MJ/s")) {
                                curUnits = "kW";
                                curConversionFactor *= 1000.0;
                            }
                            if (UtilityRoutines::SameString(curUnits, "therm/s")) {
                                curUnits = "kBtu/h";
                                curConversionFactor *= 360000.0;
                            }
                            if (UtilityRoutines::SameString(curUnits, "kBtu/s")) {
                                curUnits = "kBtu/h";
                                curConversionFactor *= 3600.0;
                            }
                            if (UtilityRoutines::SameString(curUnits, "ton-hrs/s")) {
                                curUnits = "ton";
                                curConversionFactor *= 3600.0;
                            }
                            columnHead(columnRecount) = ort->MonthlyColumns(curCol).varName + curAggString + " [" + curUnits + ']';
                            minVal = storedMaxVal;
                            maxVal = storedMinVal;
                            for (lMonth = 1; lMonth <= 12; ++lMonth) {
                                curVal = ort->MonthlyColumns(curCol).reslt(lMonth) * curConversionFactor + curConversionOffset;
                                if (ort->IsMonthGathered(lMonth)) {
                                    tableBody(columnRecount, lMonth) = RealToStr(curVal, digitsShown);
                                    if (curVal > maxVal) maxVal = curVal;
                                    if (curVal < minVal) minVal = curVal;
                                } else {
                                    tableBody(columnRecount, lMonth) = "-";
                                }
                            } // lMonth
                            // add the summary to bottom
                            if (minVal != storedMaxVal) {
                                tableBody(columnRecount, 15) = RealToStr(minVal, digitsShown);
                            }
                            if (maxVal != storedMinVal) {
                                tableBody(columnRecount, 16) = RealToStr(maxVal, digitsShown);
                            }
                        } else if ((SELECT_CASE_var == iAggType::Maximum) || (SELECT_CASE_var == iAggType::Minimum) ||
                                   (SELECT_CASE_var == iAggType::MaximumDuringHoursShown) || (SELECT_CASE_var == iAggType::MinimumDuringHoursShown)) {
                            columnRecount += 2;
                            // put in the name of the variable for the column
                            if (ort->MonthlyColumns(curCol).avgSum == OutputProcessor::StoreType::Summed) { // if it is a summed variable
                                curUnits += "/s";
                            }
                            if (UtilityRoutines::SameString(curUnits, "J/s")) {
                                curUnits = "W";
                            }
                            // CR7783 fix
                            if (UtilityRoutines::SameString(curUnits, "kWh/s")) {
                                curUnits = "W";
                                curConversionFactor *= 3600000.0;
                            }
                            if (UtilityRoutines::SameString(curUnits, "GJ/s")) {
                                curUnits = "kW";
                                curConversionFactor *= 1000000.0;
                            }
                            if (UtilityRoutines::SameString(curUnits, "MJ/s")) {
                                curUnits = "kW";
                                curConversionFactor *= 1000.0;
                            }
                            if (UtilityRoutines::SameString(curUnits, "therm/s")) {
                                curUnits = "kBtu/h";
                                curConversionFactor *= 360000.0;
                            }
                            if (UtilityRoutines::SameString(curUnits, "kBtu/s")) {
                                curUnits = "kBtu/h";
                                curConversionFactor *= 3600.0;
                            }
                            if (UtilityRoutines::SameString(curUnits, "ton-hrs/s")) {
                                curUnits = "ton";
                                curConversionFactor *= 3600.0;
                            }
                            columnHead(columnRecount - 1) = ort->MonthlyColumns(curCol).varName + curAggString + '[' + curUnits + ']';
                            columnHead(columnRecount) = ort->MonthlyColumns(curCol).varName + " {TIMESTAMP} ";
                            minVal = storedMaxVal;
                            maxVal = storedMinVal;
                            for (lMonth = 1; lMonth <= 12; ++lMonth) {
                                if (ort->IsMonthGathered(lMonth)) {
                                    curVal = ort->MonthlyColumns(curCol).reslt(lMonth);
                                    // CR7788 the conversion factors were causing an overflow for the InchPound case since the
                                    // value was very small
                                    // restructured the following lines to hide showing HUGE and -HUGE values in output table CR8154 Glazer
                                    if ((curVal < veryLarge) && (curVal > verySmall)) {
                                        curVal = curVal * curConversionFactor + curConversionOffset;
                                        if (curVal > maxVal) maxVal = curVal;
                                        if (curVal < minVal) minVal = curVal;
                                        if (curVal < veryLarge && curVal > verySmall) {
                                            tableBody(columnRecount - 1, lMonth) = RealToStr(curVal, digitsShown);
                                        } else {
                                            tableBody(columnRecount - 1, lMonth) = "-";
                                        }
                                        tableBody(columnRecount, lMonth) = DateToString(ort->MonthlyColumns(curCol).timeStamp(lMonth));
                                    } else {
                                        tableBody(columnRecount - 1, lMonth) = "-";
                                        tableBody(columnRecount, lMonth) = "-";
                                    }
                                } else {
                                    tableBody(columnRecount - 1, lMonth) = "-";
                                    tableBody(columnRecount, lMonth) = "-";
                                }
                            } // lMonth
                            // add the summary to bottom
                            // Don't include if the original min and max values are still present
                            if (minVal < veryLarge) {
                                tableBody(columnRecount - 1, 15) = RealToStr(minVal, digitsShown);
                            } else {
                                tableBody(columnRecount - 1, 15) = "-";
                            }
                            if (maxVal > verySmall) {
                                tableBody(columnRecount - 1, 16) = RealToStr(maxVal, digitsShown);
                            } else {
                                tableBody(columnRecount - 1, 15) = "-";
                            }
                        }
                    }
                } // KColumn
                WriteReportHeaders(state, ort->MonthlyInput(iInput).name, ort->MonthlyTables(curTable).keyValue, OutputProcessor::StoreType::Averaged);
                WriteSubtitle(state, "Custom Monthly Report");
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth, true); // transpose monthly XML tables.
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(
                        tableBody, rowHead, columnHead, ort->MonthlyInput(iInput).name, ort->MonthlyTables(curTable).keyValue, "Custom Monthly Report");
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                        tableBody, rowHead, columnHead, ort->MonthlyInput(iInput).name, ort->MonthlyTables(curTable).keyValue, "Custom Monthly Report");
                }
            } // jTables
        }     // iInput
    }

    void WriteTimeBinTables(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   August 2003
        //       MODIFIED       January 2010, Kyle Benne
        //                      Added SQLite output
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Set up the time bin tabular report results

        // METHODOLOGY EMPLOYED:
        //   Creates several arrays that are passed to the WriteTable
        //   routine.  All arrays are strings so numbers need to be
        //   converted prior to calling WriteTable.

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int iInObj;
        int iTable;
        int kHour;
        int kMonth;
        int nCol;
        // main table
        Array1D_string columnHead;
        Array1D_int columnWidth;
        Array1D_string rowHead(39);
        Array2D_string tableBody;
        // stat table
        Array1D_string columnHeadStat(1);
        Array1D_int columnWidthStat(1);
        Array1D_string rowHeadStat(6);
        Array2D_string tableBodyStat(1, 6);

        Real64 curIntervalStart;
        Real64 curIntervalSize;
        int curIntervalCount;
        int curResIndex;
        int curNumTables;
        int numIntervalDigits;
        int firstReport;
        Real64 topValue;
        int repIndex;
        Real64 rowTotal;
        Real64 colTotal;
        Real64 aboveTotal;
        Real64 belowTotal;
        Real64 tableTotal;
        std::string repNameWithUnitsandscheduleName;
        Real64 repStDev; // standard deviation
        Real64 repMean;
        std::string curNameWithSIUnits;
        std::string curNameAndUnits;
        int indexUnitConv;

        auto &ort(state.dataOutRptTab);

        rowHead(1) = "Interval Start";
        rowHead(2) = "Interval End";
        rowHead(3) = "January";
        rowHead(4) = "February";
        rowHead(5) = "March";
        rowHead(6) = "April";
        rowHead(7) = "May";
        rowHead(8) = "June";
        rowHead(9) = "July";
        rowHead(10) = "August";
        rowHead(11) = "September";
        rowHead(12) = "October";
        rowHead(13) = "November";
        rowHead(14) = "December";
        rowHead(15) = "12:01 to  1:00 am";
        rowHead(16) = " 1:01 to  2:00 am";
        rowHead(17) = " 2:01 to  3:00 am";
        rowHead(18) = " 3:01 to  4:00 am";
        rowHead(19) = " 4:01 to  5:00 am";
        rowHead(20) = " 5:01 to  6:00 am";
        rowHead(21) = " 6:01 to  7:00 am";
        rowHead(22) = " 7:01 to  8:00 am";
        rowHead(23) = " 8:01 to  9:00 am";
        rowHead(24) = " 9:01 to 10:00 am";
        rowHead(25) = "10:01 to 11:00 am";
        rowHead(26) = "11:01 to 12:00 pm";
        rowHead(27) = "12:01 to  1:00 pm";
        rowHead(28) = " 1:01 to  2:00 pm";
        rowHead(29) = " 2:01 to  3:00 pm";
        rowHead(30) = " 3:01 to  4:00 pm";
        rowHead(31) = " 4:01 to  5:00 pm";
        rowHead(32) = " 5:01 to  6:00 pm";
        rowHead(33) = " 6:01 to  7:00 pm";
        rowHead(34) = " 7:01 to  8:00 pm";
        rowHead(35) = " 8:01 to  9:00 pm";
        rowHead(36) = " 9:01 to 10:00 pm";
        rowHead(37) = "10:01 to 11:00 pm";
        rowHead(38) = "11:01 to 12:00 am";
        rowHead(39) = "Total";
        for (iInObj = 1; iInObj <= ort->OutputTableBinnedCount; ++iInObj) {
            firstReport = ort->OutputTableBinned(iInObj).resIndex;
            curNameWithSIUnits = ort->OutputTableBinned(iInObj).varOrMeter + unitEnumToStringBrackets(ort->OutputTableBinned(iInObj).units);
            if (ort->unitsStyle == iUnitsStyle::InchPound) {
                LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                curIntervalStart = ConvertIP(state, indexUnitConv, ort->OutputTableBinned(iInObj).intervalStart);
                curIntervalSize = ConvertIPdelta(state, indexUnitConv, ort->OutputTableBinned(iInObj).intervalSize);
            } else {
                curNameAndUnits = curNameWithSIUnits;
                curIntervalStart = ort->OutputTableBinned(iInObj).intervalStart;
                curIntervalSize = ort->OutputTableBinned(iInObj).intervalSize;
            }
            curIntervalCount = ort->OutputTableBinned(iInObj).intervalCount;
            curResIndex = ort->OutputTableBinned(iInObj).resIndex;
            curNumTables = ort->OutputTableBinned(iInObj).numTables;
            topValue = curIntervalStart + curIntervalSize * curIntervalCount;
            if (curIntervalSize < 1) {
                numIntervalDigits = 4;
            } else if (curIntervalSize >= 10) {
                numIntervalDigits = 0;
            } else {
                numIntervalDigits = 2;
            }
            // make arrays two columns wider for below and above bin range
            columnHead.allocate(curIntervalCount + 3);
            columnWidth.allocate(curIntervalCount + 3);
            columnWidth = 14; // array assignment - same for all columns
            tableBody.allocate(curIntervalCount + 3, 39);
            tableBody = "";
            columnHead = "- [hr]";
            tableBody(1, 1) = "less than";
            tableBody(1, 2) = RealToStr(curIntervalStart, numIntervalDigits);
            for (nCol = 1; nCol <= curIntervalCount; ++nCol) {
                columnHead(nCol + 1) = format("{} [hr]", nCol);
                // beginning of interval
                tableBody(nCol + 1, 1) = RealToStr(curIntervalStart + (nCol - 1) * curIntervalSize, numIntervalDigits) + "<=";
                // end of interval
                tableBody(nCol + 1, 2) = RealToStr(curIntervalStart + nCol * curIntervalSize, numIntervalDigits) + '>';
            }
            tableBody(curIntervalCount + 2, 1) = "equal to or more than";
            tableBody(curIntervalCount + 2, 2) = RealToStr(topValue, numIntervalDigits);
            tableBody(curIntervalCount + 3, 1) = "Row";
            tableBody(curIntervalCount + 3, 2) = "Total";
            for (iTable = 1; iTable <= curNumTables; ++iTable) {
                repIndex = firstReport + (iTable - 1);
                if (ort->OutputTableBinned(iInObj).scheduleIndex == 0) {
                    repNameWithUnitsandscheduleName = curNameAndUnits;
                } else {
                    repNameWithUnitsandscheduleName = curNameAndUnits + " [" + ort->OutputTableBinned(iInObj).ScheduleName + ']';
                }
                WriteReportHeaders(state, repNameWithUnitsandscheduleName, ort->BinObjVarID(repIndex).namesOfObj, ort->OutputTableBinned(iInObj).avgSum);
                for (kHour = 1; kHour <= 24; ++kHour) {
                    tableBody(1, 14 + kHour) = RealToStr(ort->BinResultsBelow(repIndex).hrly(kHour), 2);
                    tableBody(curIntervalCount + 2, 14 + kHour) = RealToStr(ort->BinResultsAbove(repIndex).hrly(kHour), 2);
                    rowTotal = ort->BinResultsBelow(repIndex).hrly(kHour) + ort->BinResultsAbove(repIndex).hrly(kHour);
                    for (nCol = 1; nCol <= curIntervalCount; ++nCol) {
                        tableBody(nCol + 1, 14 + kHour) = RealToStr(ort->BinResults(nCol, repIndex).hrly(kHour), 2);
                        // sum the total for all columns
                        rowTotal += ort->BinResults(nCol, repIndex).hrly(kHour);
                    }
                    tableBody(nCol + 2, 14 + kHour) = RealToStr(rowTotal, 2);
                }
                tableTotal = 0.0;
                for (kMonth = 1; kMonth <= 12; ++kMonth) {
                    tableBody(1, 2 + kMonth) = RealToStr(ort->BinResultsBelow(repIndex).mnth(kMonth), 2);
                    tableBody(curIntervalCount + 2, 2 + kMonth) = RealToStr(ort->BinResultsAbove(repIndex).mnth(kMonth), 2);
                    rowTotal = ort->BinResultsBelow(repIndex).mnth(kMonth) + ort->BinResultsAbove(repIndex).mnth(kMonth);
                    for (nCol = 1; nCol <= curIntervalCount; ++nCol) {
                        tableBody(nCol + 1, 2 + kMonth) = RealToStr(ort->BinResults(nCol, repIndex).mnth(kMonth), 2);
                        // sum the total for all columns
                        rowTotal += ort->BinResults(nCol, repIndex).mnth(kMonth);
                    }
                    tableBody(nCol + 2, 2 + kMonth) = RealToStr(rowTotal, 2);
                    tableTotal += rowTotal;
                }
                // compute total row
                for (nCol = 1; nCol <= curIntervalCount; ++nCol) {
                    colTotal = 0.0;
                    for (kMonth = 1; kMonth <= 12; ++kMonth) {
                        colTotal += ort->BinResults(nCol, repIndex).mnth(kMonth);
                    }
                    tableBody(nCol + 1, 39) = RealToStr(colTotal, 2);
                }
                aboveTotal = 0.0;
                belowTotal = 0.0;
                for (kMonth = 1; kMonth <= 12; ++kMonth) {
                    aboveTotal += ort->BinResultsAbove(repIndex).mnth(kMonth);
                    belowTotal += ort->BinResultsBelow(repIndex).mnth(kMonth);
                }
                tableBody(1, 39) = RealToStr(belowTotal, 2);
                tableBody(curIntervalCount + 2, 39) = RealToStr(aboveTotal, 2);
                tableBody(curIntervalCount + 3, 39) = RealToStr(tableTotal, 2);
                WriteTextLine(state, "Values in table are in hours.");
                WriteTextLine(state, "");
                WriteSubtitle(state, "Time Bin Results");
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth, true); // transpose XML tables
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(
                        tableBody, rowHead, columnHead, repNameWithUnitsandscheduleName, ort->BinObjVarID(repIndex).namesOfObj, "Time Bin Results");
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                        tableBody, rowHead, columnHead, repNameWithUnitsandscheduleName, ort->BinObjVarID(repIndex).namesOfObj, "Time Bin Results");
                }
                // create statistics table
                rowHeadStat(1) = "Minimum";
                rowHeadStat(2) = "Mean minus two standard deviations";
                rowHeadStat(3) = "Mean";
                rowHeadStat(4) = "Mean plus two standard deviations";
                rowHeadStat(5) = "Maximum";
                rowHeadStat(6) = "Standard deviation";
                columnHeadStat(1) = "Statistic";
                columnWidthStat(1) = 14;
                // per Applied Regression Analysis and Other Multivariate Methods, Kleinburger/Kupper, 1978
                // first check if very large constant number has caused the second part to be larger than the first
                if (ort->BinStatistics(repIndex).n > 1) {
                    if (ort->BinStatistics(repIndex).sum2 > (pow_2(ort->BinStatistics(repIndex).sum) / ort->BinStatistics(repIndex).n)) {
                        repStDev = std::sqrt((ort->BinStatistics(repIndex).sum2 - (pow_2(ort->BinStatistics(repIndex).sum) / ort->BinStatistics(repIndex).n)) /
                                             (ort->BinStatistics(repIndex).n - 1));
                    } else {
                        repStDev = 0.0;
                    }
                    repMean = ort->BinStatistics(repIndex).sum / ort->BinStatistics(repIndex).n;
                } else {
                    repStDev = 0.0;
                    repMean = 0.0;
                }
                if (ort->unitsStyle == iUnitsStyle::InchPound) {
                    tableBodyStat(1, 1) = RealToStr(ConvertIP(state, indexUnitConv, ort->BinStatistics(repIndex).minimum), 2);
                    tableBodyStat(1, 2) = RealToStr(ConvertIP(state, indexUnitConv, repMean - 2 * repStDev), 2);
                    tableBodyStat(1, 3) = RealToStr(ConvertIP(state, indexUnitConv, repMean), 2);
                    tableBodyStat(1, 4) = RealToStr(ConvertIP(state, indexUnitConv, repMean + 2 * repStDev), 2);
                    tableBodyStat(1, 5) = RealToStr(ConvertIP(state, indexUnitConv, ort->BinStatistics(repIndex).maximum), 2);
                    tableBodyStat(1, 6) = RealToStr(ConvertIPdelta(state, indexUnitConv, repStDev), 2);
                } else {
                    tableBodyStat(1, 1) = RealToStr(ort->BinStatistics(repIndex).minimum, 2);
                    tableBodyStat(1, 2) = RealToStr(repMean - 2 * repStDev, 2);
                    tableBodyStat(1, 3) = RealToStr(repMean, 2);
                    tableBodyStat(1, 4) = RealToStr(repMean + 2 * repStDev, 2);
                    tableBodyStat(1, 5) = RealToStr(ort->BinStatistics(repIndex).maximum, 2);
                    tableBodyStat(1, 6) = RealToStr(repStDev, 2);
                }
                WriteSubtitle(state, "Statistics");
                WriteTable(state, tableBodyStat, rowHeadStat, columnHeadStat, columnWidthStat, true); // transpose XML table
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(
                        tableBody, rowHead, columnHead, repNameWithUnitsandscheduleName, ort->BinObjVarID(repIndex).namesOfObj, "Statistics");
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                        tableBody, rowHead, columnHead, repNameWithUnitsandscheduleName, ort->BinObjVarID(repIndex).namesOfObj, "Statistics");
                }
            }
        }
    }

    void WriteBEPSTable(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   November 2003
        //       MODIFIED       January 2010, Kyle Benne; Added SQLite output
        //                      March 2020, Dareum Nam; Disaggregated "Additional Fuel"
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Take the gathered total and enduse meter data and structure
        //   the results into a tabular report for output.

        // METHODOLOGY EMPLOYED:
        //   Create arrays for the call to WriteTable and then call it.
        //   This report actually consists of many sub-tables each with
        //   its own call to WriteTable.  Anytime that column headings are
        //   desired they are done in a new table because the only place
        //   that will split up very long header lines for the fixed width
        //   table is the header rows.

        // REFERENCES:
        // na

        // Using/Aliasing
        using DataHVACGlobals::deviationFromSetPtThresholdClg;
        using DataHVACGlobals::deviationFromSetPtThresholdHtg;
        using DataWater::StorageTankDataStruct;
        using ScheduleManager::GetScheduleName;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        int const colElectricity(1);
        int const colGas(2);
        int const colPurchCool(11);
        int const colPurchHeat(12);

        Real64 const SmallValue(1.e-14);
        auto &ort(state.dataOutRptTab);

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:

        // all arrays are in the format: (row, column)
        Array1D_string columnHead;
        Array1D_int columnWidth;
        Array1D_string rowHead;
        Array2D_string tableBody;

        // all arrays are in the format: (row, columnm)
        Array2D<Real64> useVal(13, 15);
        Array2D<Real64> normalVal(13, 4);
        Array1D<Real64> collapsedTotal(13);
        Array2D<Real64> collapsedEndUse(13, DataGlobalConstants::iEndUse.size());
        Array3D<Real64> collapsedEndUseSub(state.dataOutputProcessor->MaxNumSubcategories, DataGlobalConstants::iEndUse.size(), 13);
        Array2D<Real64> endUseSubOther(13, DataGlobalConstants::iEndUse.size());
        Real64 totalOnsiteHeat;
        Real64 totalOnsiteWater;
        Real64 totalWater;
        Real64 netElecPurchasedSold;
        Real64 totalSiteEnergyUse;
        Real64 netSiteEnergyUse;
        Real64 totalSourceEnergyUse;
        Real64 netSourceEnergyUse;
        Real64 netSourceElecPurchasedSold;
        int iResource;
        int kEndUseSub;
        int i;
        Real64 largeConversionFactor;
        Real64 kConversionFactor;
        int numRows;
        Real64 initialStorage;
        Real64 finalStorage;
        Real64 StorageChange;
        int resourcePrimaryHeating;
        Real64 heatingMaximum;
        std::string footnote;
        Real64 waterConversionFactor;
        Real64 areaConversionFactor;
        Real64 convBldgGrossFloorArea;
        Real64 convBldgCondFloorArea;
        std::string curNameWithSIUnits;
        std::string curNameAndUnits;
        int indexUnitConv;
        Real64 processFraction;
        Real64 processElecCost;
        Real64 processGasCost;
        Real64 processOthrCost;
        Real64 useValColAddFuel15;
        Real64 useValColAddFuel5;
        Real64 useValColAddFuel13;

        std::string subCatName;
        static Real64 leedSiteIntLite(0.0);
        static Real64 leedSiteSpHeat(0.0);
        static Real64 leedSiteSpCool(0.0);
        static Real64 leedSiteFanInt(0.0);
        static Real64 leedSiteSrvWatr(0.0);
        static Real64 leedSiteRecept(0.0);
        static Real64 leedSiteTotal(0.0);
        Real64 unconvert;

        if (ort->displayTabularBEPS || ort->displayLEEDSummary) {
            // show the headers of the report
            if (ort->displayTabularBEPS) {
                WriteReportHeaders(state, "Annual Building Utility Performance Summary", "Entire Facility", OutputProcessor::StoreType::Averaged);
                // show the number of hours that the table applies to
                WriteTextLine(state, "Values gathered over " + RealToStr(ort->gatherElapsedTimeBEPS, 2) + " hours", true);
                if (ort->gatherElapsedTimeBEPS < 8759.0) { // might not add up to 8760 exactly but can't be more than 1 hour diff.
                    WriteTextLine(state, "WARNING: THE REPORT DOES NOT REPRESENT A FULL ANNUAL SIMULATION.", true);
                }
                WriteTextLine(state, "", true);
            }
            // determine building floor areas
            DetermineBuildingFloorArea(state);
            // collapse the gatherEndUseBEPS array to the resource groups displayed
            for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                collapsedEndUse(1, jEndUse) = ort->gatherEndUseBEPS(1, jEndUse);   // electricity
                collapsedEndUse(2, jEndUse) = ort->gatherEndUseBEPS(2, jEndUse);   // natural gas
                collapsedEndUse(3, jEndUse) = ort->gatherEndUseBEPS(6, jEndUse);   // gasoline
                collapsedEndUse(4, jEndUse) = ort->gatherEndUseBEPS(8, jEndUse);   // diesel
                collapsedEndUse(5, jEndUse) = ort->gatherEndUseBEPS(9, jEndUse);   // coal
                collapsedEndUse(6, jEndUse) = ort->gatherEndUseBEPS(10, jEndUse);  // Fuel Oil No1
                collapsedEndUse(7, jEndUse) = ort->gatherEndUseBEPS(11, jEndUse);  // Fuel Oil No2
                collapsedEndUse(8, jEndUse) = ort->gatherEndUseBEPS(12, jEndUse);  // propane
                collapsedEndUse(9, jEndUse) = ort->gatherEndUseBEPS(13, jEndUse);  // otherfuel1
                collapsedEndUse(10, jEndUse) = ort->gatherEndUseBEPS(14, jEndUse); // otherfuel2
                collapsedEndUse(11, jEndUse) = ort->gatherEndUseBEPS(3, jEndUse);  // district cooling <- purchased cooling
                collapsedEndUse(12, jEndUse) =
                    ort->gatherEndUseBEPS(4, jEndUse) + ort->gatherEndUseBEPS(5, jEndUse); // district heating <- purchased heating | <- steam
                collapsedEndUse(13, jEndUse) = ort->gatherEndUseBEPS(7, jEndUse);     // water
            }
            // repeat with totals
            collapsedTotal(1) = ort->gatherTotalsBEPS(1);                        // electricity
            collapsedTotal(2) = ort->gatherTotalsBEPS(2);                        // natural gas
            collapsedTotal(3) = ort->gatherTotalsBEPS(6);                        // gasoline
            collapsedTotal(4) = ort->gatherTotalsBEPS(8);                        // diesel
            collapsedTotal(5) = ort->gatherTotalsBEPS(9);                        // coal
            collapsedTotal(6) = ort->gatherTotalsBEPS(10);                       // Fuel Oil No1
            collapsedTotal(7) = ort->gatherTotalsBEPS(11);                       // Fuel Oil No2
            collapsedTotal(8) = ort->gatherTotalsBEPS(12);                       // propane
            collapsedTotal(9) = ort->gatherTotalsBEPS(13);                       // other fuel 1
            collapsedTotal(10) = ort->gatherTotalsBEPS(14);                      // other fuel 2
            collapsedTotal(11) = ort->gatherTotalsBEPS(3);                       // district cooling <- purchased cooling
            collapsedTotal(12) = ort->gatherTotalsBEPS(4) + ort->gatherTotalsBEPS(5); // district heating <- purchased heating | <- steam
            collapsedTotal(13) = ort->gatherTotalsBEPS(7);                       // water

            if (state.dataGlobal->createPerfLog) {
                UtilityRoutines::appendPerfLog(state, "Electricity ABUPS Total [J]", format("{:.3R}", collapsedTotal(1)));
                UtilityRoutines::appendPerfLog(state, "Natural Gas ABUPS Total [J]", format("{:.3R}", collapsedTotal(2)));
                UtilityRoutines::appendPerfLog(state, "Gasoline ABUPS Total [J]", format("{:.3R}", collapsedTotal(3)));
                UtilityRoutines::appendPerfLog(state, "Diesel ABUPS Total [J]", format("{:.3R}", collapsedTotal(4)));
                UtilityRoutines::appendPerfLog(state, "Coal ABUPS Total [J]", format("{:.3R}", collapsedTotal(5)));
                UtilityRoutines::appendPerfLog(state, "Fuel Oil No 1 ABUPS Total [J]", format("{:.3R}", collapsedTotal(6)));
                UtilityRoutines::appendPerfLog(state, "Fuel Oil No 2 ABUPS Total [J]", format("{:.3R}", collapsedTotal(7)));
                UtilityRoutines::appendPerfLog(state, "Propane ABUPS Total [J]", format("{:.3R}", collapsedTotal(8)));
                UtilityRoutines::appendPerfLog(state, "Other Fuel 1 ABUPS Total [J]", format("{:.3R}", collapsedTotal(9)));
                UtilityRoutines::appendPerfLog(state, "Other Fuel 2 ABUPS Total [J]", format("{:.3R}", collapsedTotal(10)));
                UtilityRoutines::appendPerfLog(state, "District Cooling ABUPS Total [J]", format("{:.3R}", collapsedTotal(11)));
                UtilityRoutines::appendPerfLog(state, "District Heating ABUPS Total [J]", format("{:.3R}", collapsedTotal(12)));
                UtilityRoutines::appendPerfLog(state, "Water ABUPS Total [m3]", format("{:.3R}", collapsedTotal(13)));
                UtilityRoutines::appendPerfLog(state, "Values Gathered Over [hours]", format("{:.2R}", ort->gatherElapsedTimeBEPS));
                UtilityRoutines::appendPerfLog(state,
                                               "Facility Any Zone Oscillating Temperatures Time [hours]",
                                               format("{:.2R}", state.dataZoneTempPredictorCorrector->AnnualAnyZoneTempOscillate));
                UtilityRoutines::appendPerfLog(state,
                                               "Facility Any Zone Oscillating Temperatures During Occupancy Time [hours]",
                                               format("{:.2R}", state.dataZoneTempPredictorCorrector->AnnualAnyZoneTempOscillateDuringOccupancy));
                UtilityRoutines::appendPerfLog(state,
                                               "Facility Any Zone Oscillating Temperatures in Deadband Time [hours]",
                                               format("{:.2R}", state.dataZoneTempPredictorCorrector->AnnualAnyZoneTempOscillateInDeadband));
            }
            for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                    collapsedEndUseSub(kEndUseSub, jEndUse, 1) = ort->gatherEndUseSubBEPS(kEndUseSub, jEndUse, 1);   // electricity
                    collapsedEndUseSub(kEndUseSub, jEndUse, 2) = ort->gatherEndUseSubBEPS(kEndUseSub, jEndUse, 2);   // natural gas
                    collapsedEndUseSub(kEndUseSub, jEndUse, 3) = ort->gatherEndUseSubBEPS(kEndUseSub, jEndUse, 6);   // gasoline
                    collapsedEndUseSub(kEndUseSub, jEndUse, 4) = ort->gatherEndUseSubBEPS(kEndUseSub, jEndUse, 8);   // diesel
                    collapsedEndUseSub(kEndUseSub, jEndUse, 5) = ort->gatherEndUseSubBEPS(kEndUseSub, jEndUse, 9);   // coal
                    collapsedEndUseSub(kEndUseSub, jEndUse, 6) = ort->gatherEndUseSubBEPS(kEndUseSub, jEndUse, 10);  // Fuel Oil No1
                    collapsedEndUseSub(kEndUseSub, jEndUse, 7) = ort->gatherEndUseSubBEPS(kEndUseSub, jEndUse, 11);  // Fuel Oil No2
                    collapsedEndUseSub(kEndUseSub, jEndUse, 8) = ort->gatherEndUseSubBEPS(kEndUseSub, jEndUse, 12);  // propane
                    collapsedEndUseSub(kEndUseSub, jEndUse, 9) = ort->gatherEndUseSubBEPS(kEndUseSub, jEndUse, 13);  // otherfuel1
                    collapsedEndUseSub(kEndUseSub, jEndUse, 10) = ort->gatherEndUseSubBEPS(kEndUseSub, jEndUse, 14); // otherfuel2
                    collapsedEndUseSub(kEndUseSub, jEndUse, 11) = ort->gatherEndUseSubBEPS(kEndUseSub, jEndUse, 3);  // district cooling <- purch cooling
                    collapsedEndUseSub(kEndUseSub, jEndUse, 12) =
                        ort->gatherEndUseSubBEPS(kEndUseSub, jEndUse, 4) +
                        ort->gatherEndUseSubBEPS(kEndUseSub, jEndUse, 5); // district heating <- purch heating | <- steam
                    collapsedEndUseSub(kEndUseSub, jEndUse, 13) = ort->gatherEndUseSubBEPS(kEndUseSub, jEndUse, 7); // water
                }
            }

            // unit conversion - all values are used as divisors
            {
                auto const SELECT_CASE_var(ort->unitsStyle);
                if (SELECT_CASE_var == iUnitsStyle::JtoKWH) {
                    largeConversionFactor = 3600000.0;
                    kConversionFactor = 1.0;
                    waterConversionFactor = 1.0;
                    areaConversionFactor = 1.0;
                } else if (SELECT_CASE_var == iUnitsStyle::InchPound) {
                    largeConversionFactor = getSpecificUnitDivider(state, "J", "kBtu"); // 1054351.84 J to kBtu
                    kConversionFactor = 1.0;
                    waterConversionFactor = getSpecificUnitDivider(state, "m3", "gal"); // 0.003785413 m3 to gal
                    areaConversionFactor = getSpecificUnitDivider(state, "m2", "ft2");  // 0.092893973 m2 to ft2
                } else {
                    largeConversionFactor = 1000000000.0;
                    kConversionFactor = 1000.0;
                    waterConversionFactor = 1.0;
                    areaConversionFactor = 1.0;
                }
            }

            // convert floor areas
            convBldgGrossFloorArea = ort->buildingGrossFloorArea / areaConversionFactor;
            convBldgCondFloorArea = ort->buildingConditionedFloorArea / areaConversionFactor;

            // convert units into GJ (divide by 1,000,000,000) if J otherwise kWh
            for (iResource = 1; iResource <= 12; ++iResource) { // don't do water
                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    collapsedEndUse(iResource, jEndUse) /= largeConversionFactor;
                    for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                        collapsedEndUseSub(kEndUseSub, jEndUse, iResource) /= largeConversionFactor;
                    }
                }
                collapsedTotal(iResource) /= largeConversionFactor;
            }
            // do water
            for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                collapsedEndUse(13, jEndUse) /= waterConversionFactor;
                for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                    collapsedEndUseSub(kEndUseSub, jEndUse, 13) /= waterConversionFactor;
                }
            }

            collapsedTotal(13) = WaterConversionFunct(collapsedTotal(13), waterConversionFactor);

            // convert to GJ
            ort->gatherPowerFuelFireGen /= largeConversionFactor;
            ort->gatherPowerPV /= largeConversionFactor;
            ort->gatherPowerWind /= largeConversionFactor;
            ort->gatherPowerHTGeothermal /= largeConversionFactor;
            ort->gatherPowerConversion /= largeConversionFactor;
            ort->gatherElecProduced /= largeConversionFactor;
            ort->gatherElecPurchased /= largeConversionFactor;
            ort->gatherElecSurplusSold /= largeConversionFactor;

            // get change in overall state of charge for electrical storage devices.
            if (facilityElectricServiceObj->numElecStorageDevices > 0) {
                // All flow in/out of storage is accounted for in gatherElecStorage, so separate calculation of change in state of charge is not
                // necessary OverallNetEnergyFromStorage = ( sum( ElecStorage.StartingEnergyStored() ) - sum( ElecStorage.ThisTimeStepStateOfCharge()
                // ) ) + gatherElecStorage;
                ort->OverallNetEnergyFromStorage = ort->gatherElecStorage;
                ort->OverallNetEnergyFromStorage /= largeConversionFactor;
            } else {
                ort->OverallNetEnergyFromStorage = 0.0;
            }
            // determine which resource is the primary heating resourse
            resourcePrimaryHeating = 0;
            heatingMaximum = 0.0;
            for (iResource = 1; iResource <= 12; ++iResource) { // don't do water
                if (collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Heating)) > heatingMaximum) {
                    heatingMaximum = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Heating));
                    resourcePrimaryHeating = iResource;
                }
            }

            //---- Source and Site Energy Sub-Table
            rowHead.allocate(4);
            columnHead.allocate(3);
            columnWidth.allocate(3);
            columnWidth = 14; // array assignment - same for all columns
            tableBody.allocate(3, 4);

            {
                auto const SELECT_CASE_var(ort->unitsStyle);
                if (SELECT_CASE_var == iUnitsStyle::JtoKWH) {
                    columnHead(1) = "Total Energy [kWh]";
                    columnHead(2) = "Energy Per Total Building Area [kWh/m2]";
                    columnHead(3) = "Energy Per Conditioned Building Area [kWh/m2]";
                } else if (SELECT_CASE_var == iUnitsStyle::InchPound) {
                    columnHead(1) = "Total Energy [kBtu]";
                    columnHead(2) = "Energy Per Total Building Area [kBtu/ft2]";
                    columnHead(3) = "Energy Per Conditioned Building Area [kBtu/ft2]";
                } else {
                    columnHead(1) = "Total Energy [GJ]";
                    columnHead(2) = "Energy Per Total Building Area [MJ/m2]";
                    columnHead(3) = "Energy Per Conditioned Building Area [MJ/m2]";
                }
            }

            rowHead(1) = "Total Site Energy";
            rowHead(2) = "Net Site Energy";
            rowHead(3) = "Total Source Energy";
            rowHead(4) = "Net Source Energy";

            tableBody = "";

            // compute the net amount of electricity received from the utility which
            // is the amount purchased less the amount sold to the utility. Note that
            // previously these variables were converted into GJ so now we don't need
            // to do any conversion
            // water is not included   gatherTotalsBEPS(7)    !water
            totalSiteEnergyUse = (ort->gatherTotalsBEPS(1) + ort->gatherTotalsBEPS(2) + ort->gatherTotalsBEPS(3) + ort->gatherTotalsBEPS(4) + ort->gatherTotalsBEPS(5) +
                                  ort->gatherTotalsBEPS(6) + ort->gatherTotalsBEPS(8) + ort->gatherTotalsBEPS(9) + ort->gatherTotalsBEPS(10) + ort->gatherTotalsBEPS(11) +
                                  ort->gatherTotalsBEPS(12) + ort->gatherTotalsBEPS(13) + ort->gatherTotalsBEPS(14)) /
                                 largeConversionFactor; // electricity | natural gas | district cooling | district heating | steam | gasoline | diesel
                                                        // | coal | Fuel Oil No1 | Fuel Oil No2 | propane | otherfuel1 | otherfuel2

            netElecPurchasedSold = ort->gatherElecPurchased - ort->gatherElecSurplusSold;

            // water is not included   gatherTotalsBEPS(7)    !water
            netSiteEnergyUse = netElecPurchasedSold + (ort->gatherTotalsBEPS(2) + ort->gatherTotalsBEPS(3) + ort->gatherTotalsBEPS(4) + ort->gatherTotalsBEPS(5) +
                                                       ort->gatherTotalsBEPS(6) + ort->gatherTotalsBEPS(8) + ort->gatherTotalsBEPS(9) + ort->gatherTotalsBEPS(10) +
                                                       ort->gatherTotalsBEPS(11) + ort->gatherTotalsBEPS(12) + ort->gatherTotalsBEPS(13) + ort->gatherTotalsBEPS(14)) /
                                                          largeConversionFactor; // electricity (already in GJ) | natural gas | district cooling |
                                                                                 // district heating | steam | gasoline | diesel | coal | Fuel Oil No1
                                                                                 // | Fuel Oil No2 | propane | otherfuel1 | otherfuel2

            if (ort->efficiencyDistrictCooling == 0) ort->efficiencyDistrictCooling = 1.0;
            if (ort->efficiencyDistrictHeating == 0) ort->efficiencyDistrictHeating = 1.0;

            // source emissions already have the source factors included in the calcs.
            totalSourceEnergyUse = 0.0;
            //  electricity
            if (ort->fuelfactorsused(1)) {
                totalSourceEnergyUse += ort->gatherTotalsSource(1);
            } else {
                totalSourceEnergyUse += ort->gatherTotalsBEPS(1) * ort->sourceFactorElectric;
            }
            //  natural gas
            if (ort->fuelfactorsused(2)) {
                totalSourceEnergyUse += ort->gatherTotalsSource(2);
            } else {
                totalSourceEnergyUse += ort->gatherTotalsBEPS(2) * ort->sourceFactorNaturalGas;
            }
            // gasoline
            if (ort->fuelfactorsused(3)) {
                totalSourceEnergyUse += ort->gatherTotalsSource(3);
            } else {
                totalSourceEnergyUse += ort->gatherTotalsBEPS(6) * ort->sourceFactorGasoline;
            }
            // diesel
            if (ort->fuelfactorsused(4)) {
                totalSourceEnergyUse += ort->gatherTotalsSource(4);
            } else {
                totalSourceEnergyUse += ort->gatherTotalsBEPS(8) * ort->sourceFactorDiesel;
            }
            // coal
            if (ort->fuelfactorsused(5)) {
                totalSourceEnergyUse += ort->gatherTotalsSource(5);
            } else {
                totalSourceEnergyUse += ort->gatherTotalsBEPS(9) * ort->sourceFactorCoal;
            }
            // Fuel Oil No1
            if (ort->fuelfactorsused(6)) {
                totalSourceEnergyUse += ort->gatherTotalsSource(6);
            } else {
                totalSourceEnergyUse += ort->gatherTotalsBEPS(10) * ort->sourceFactorFuelOil1;
            }
            // Fuel Oil No2
            if (ort->fuelfactorsused(7)) {
                totalSourceEnergyUse += ort->gatherTotalsSource(7);
            } else {
                totalSourceEnergyUse += ort->gatherTotalsBEPS(11) * ort->sourceFactorFuelOil2;
            }
            // propane
            if (ort->fuelfactorsused(8)) {
                totalSourceEnergyUse += ort->gatherTotalsSource(8);
            } else {
                totalSourceEnergyUse += ort->gatherTotalsBEPS(12) * ort->sourceFactorPropane;
            }
            // otherfuel1
            if (ort->fuelfactorsused(11)) {
                totalSourceEnergyUse += ort->gatherTotalsSource(11);
            } else {
                totalSourceEnergyUse += ort->gatherTotalsBEPS(13) * ort->sourceFactorOtherFuel1;
            }
            // otherfuel2
            if (ort->fuelfactorsused(12)) {
                totalSourceEnergyUse += ort->gatherTotalsSource(12);
            } else {
                totalSourceEnergyUse += ort->gatherTotalsBEPS(14) * ort->sourceFactorOtherFuel2;
            }

            totalSourceEnergyUse =
                (totalSourceEnergyUse + ort->gatherTotalsBEPS(3) * ort->sourceFactorElectric / ort->efficiencyDistrictCooling +
                 ort->gatherTotalsBEPS(4) * ort->sourceFactorNaturalGas / ort->efficiencyDistrictHeating + ort->gatherTotalsBEPS(5) * ort->sourceFactorSteam) /
                largeConversionFactor; // district cooling | district heating | steam

            // now determine "net" source from purchased and surplus sold (still in J)

            if (ort->fuelfactorsused(1)) {
                netSourceElecPurchasedSold = ort->gatherTotalsSource(9) - ort->gatherTotalsSource(10);
            } else {
                netSourceElecPurchasedSold = netElecPurchasedSold * ort->sourceFactorElectric * largeConversionFactor; // back to J
            }

            netSourceEnergyUse = 0.0;
            //  natural gas
            if (ort->fuelfactorsused(2)) {
                netSourceEnergyUse += ort->gatherTotalsSource(2);
            } else {
                netSourceEnergyUse += ort->gatherTotalsBEPS(2) * ort->sourceFactorNaturalGas;
            }
            // gasoline
            if (ort->fuelfactorsused(3)) {
                netSourceEnergyUse += ort->gatherTotalsSource(3);
            } else {
                netSourceEnergyUse += ort->gatherTotalsBEPS(6) * ort->sourceFactorGasoline;
            }
            // diesel
            if (ort->fuelfactorsused(4)) {
                netSourceEnergyUse += ort->gatherTotalsSource(4);
            } else {
                netSourceEnergyUse += ort->gatherTotalsBEPS(8) * ort->sourceFactorDiesel;
            }
            // coal
            if (ort->fuelfactorsused(5)) {
                netSourceEnergyUse += ort->gatherTotalsSource(5);
            } else {
                netSourceEnergyUse += ort->gatherTotalsBEPS(9) * ort->sourceFactorCoal;
            }
            // Fuel Oil No1
            if (ort->fuelfactorsused(6)) {
                netSourceEnergyUse += ort->gatherTotalsSource(6);
            } else {
                netSourceEnergyUse += ort->gatherTotalsBEPS(10) * ort->sourceFactorFuelOil1;
            }
            // Fuel Oil No2
            if (ort->fuelfactorsused(7)) {
                netSourceEnergyUse += ort->gatherTotalsSource(7);
            } else {
                netSourceEnergyUse += ort->gatherTotalsBEPS(11) * ort->sourceFactorFuelOil2;
            }
            // propane
            if (ort->fuelfactorsused(8)) {
                netSourceEnergyUse += ort->gatherTotalsSource(8);
            } else {
                netSourceEnergyUse += ort->gatherTotalsBEPS(12) * ort->sourceFactorPropane;
            }
            // otherfuel1
            if (ort->fuelfactorsused(11)) {
                netSourceEnergyUse += ort->gatherTotalsSource(11);
            } else {
                netSourceEnergyUse += ort->gatherTotalsBEPS(13) * ort->sourceFactorOtherFuel1;
            }
            // otherfuel2
            if (ort->fuelfactorsused(12)) {
                netSourceEnergyUse += ort->gatherTotalsSource(12);
            } else {
                netSourceEnergyUse += ort->gatherTotalsBEPS(14) * ort->sourceFactorOtherFuel2;
            }

            netSourceEnergyUse =
                (netSourceEnergyUse + netSourceElecPurchasedSold + ort->gatherTotalsBEPS(3) * ort->sourceFactorElectric / ort->efficiencyDistrictCooling +
                 ort->gatherTotalsBEPS(4) * ort->sourceFactorNaturalGas / ort->efficiencyDistrictHeating + ort->gatherTotalsBEPS(5) * ort->sourceFactorSteam) /
                largeConversionFactor; // from other fuels | net source from electricity | district cooling | district heating | steam

            // show annual values
            tableBody(1, 1) = RealToStr(totalSiteEnergyUse, 2);
            tableBody(1, 2) = RealToStr(netSiteEnergyUse, 2);
            tableBody(1, 3) = RealToStr(totalSourceEnergyUse, 2);
            tableBody(1, 4) = RealToStr(netSourceEnergyUse, 2);
            // show  per building area
            if (convBldgGrossFloorArea > 0) {
                tableBody(2, 1) = RealToStr(totalSiteEnergyUse * kConversionFactor / convBldgGrossFloorArea, 2);
                tableBody(2, 2) = RealToStr(netSiteEnergyUse * kConversionFactor / convBldgGrossFloorArea, 2);
                tableBody(2, 3) = RealToStr(totalSourceEnergyUse * kConversionFactor / convBldgGrossFloorArea, 2);
                tableBody(2, 4) = RealToStr(netSourceEnergyUse * kConversionFactor / convBldgGrossFloorArea, 2);
            }
            // show  per conditioned building area
            if (convBldgCondFloorArea > 0) {
                tableBody(3, 1) = RealToStr(totalSiteEnergyUse * kConversionFactor / convBldgCondFloorArea, 2);
                tableBody(3, 2) = RealToStr(netSiteEnergyUse * kConversionFactor / convBldgCondFloorArea, 2);
                tableBody(3, 3) = RealToStr(totalSourceEnergyUse * kConversionFactor / convBldgCondFloorArea, 2);
                tableBody(3, 4) = RealToStr(netSourceEnergyUse * kConversionFactor / convBldgCondFloorArea, 2);
            }

            // heading for the entire sub-table
            if (ort->displayTabularBEPS) {
                WriteSubtitle(state, "Site and Source Energy");
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(
                        tableBody, rowHead, columnHead, "AnnualBuildingUtilityPerformanceSummary", "Entire Facility", "Site and Source Energy");
                }

                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                        tableBody, rowHead, columnHead, "Annual Building Utility Performance Summary", "Entire Facility", "Site and Source Energy");
                }
            }

            //---- Source and Site Energy Sub-Table
            rowHead.allocate(13);
            columnHead.allocate(1);
            columnWidth.allocate(1);
            columnWidth = 50; // array assignment
            tableBody.allocate(1, 13);

            columnHead(1) = "Site=>Source Conversion Factor";

            rowHead(1) = "Electricity";
            rowHead(2) = "Natural Gas";
            rowHead(3) = "District Cooling";
            rowHead(4) = "District Heating";
            rowHead(5) = "Steam";
            rowHead(6) = "Gasoline";
            rowHead(7) = "Diesel";
            rowHead(8) = "Coal";
            rowHead(9) = "Fuel Oil No 1";
            rowHead(10) = "Fuel Oil No 2";
            rowHead(11) = "Propane";
            rowHead(12) = "Other Fuel 1";
            rowHead(13) = "Other Fuel 2";

            tableBody = "";

            // set columns to conversion factors
            // show values
            //  tableBody(1,1)  = TRIM(RealToStr(sourceFactorElectric,3))
            //  tableBody(2,1)  = TRIM(RealToStr(sourceFactorNaturalGas, 3))
            //  tableBody(3,1)  = TRIM(RealToStr(sourceFactorElectric/ efficiencyDistrictCooling,3))
            //  tableBody(4,1)  = TRIM(RealToStr(sourceFactorNaturalGas/ efficiencyDistrictHeating ,3))
            //  tableBody(5,1)  = TRIM(RealToStr(sourceFactorSteam ,3))
            //  tableBody(6,1)  = TRIM(RealToStr(sourceFactorGasoline ,3))
            //  tableBody(7,1)  = TRIM(RealToStr(sourceFactorDiesel ,3))
            //  tableBody(8,1)  = TRIM(RealToStr(sourceFactorCoal ,3))
            //  tableBody(9,1)  = TRIM(RealToStr(sourceFactorFuelOil1 ,3))
            //  tableBody(10,1) = TRIM(RealToStr(sourceFactorFuelOil2 ,3))
            //  tableBody(11,1) = TRIM(RealToStr(sourceFactorPropane ,3))

            if (!ort->ffSchedUsed(1)) {
                tableBody(1, 1) = RealToStr(ort->sourceFactorElectric, 3);
            } else if (ort->gatherTotalsBEPS(1) > SmallValue) {
                tableBody(1, 1) = "Effective Factor = " + RealToStr(ort->gatherTotalsBySourceBEPS(1) / ort->gatherTotalsBEPS(1), 3) +
                                  " (calculated using schedule \"" + GetScheduleName(state, ort->ffSchedIndex(1)) + "\")";
            } else {
                tableBody(1, 1) = "N/A";
            }

            if (!ort->ffSchedUsed(2)) {
                tableBody(1, 2) = RealToStr(ort->sourceFactorNaturalGas, 3);
            } else if (ort->gatherTotalsBEPS(2) > SmallValue) {
                tableBody(1, 2) = "Effective Factor = " + RealToStr(ort->gatherTotalsBySourceBEPS(2) / ort->gatherTotalsBEPS(2), 3) +
                                  " (calculated using schedule \"" + GetScheduleName(state, ort->ffSchedIndex(2)) + "\")";
            } else {
                tableBody(1, 2) = "N/A";
            }

            tableBody(1, 3) = RealToStr(ort->sourceFactorElectric / ort->efficiencyDistrictCooling, 3); // District Cooling

            tableBody(1, 4) = RealToStr(ort->sourceFactorNaturalGas / ort->efficiencyDistrictHeating, 3); // District Heating

            tableBody(1, 5) = RealToStr(ort->sourceFactorSteam, 3); // Steam

            if (!ort->ffSchedUsed(6)) {
                tableBody(1, 6) = RealToStr(ort->sourceFactorGasoline, 3);
            } else if (ort->gatherTotalsBEPS(6) > SmallValue) {
                tableBody(1, 6) = "Effective Factor = " + RealToStr(ort->gatherTotalsBySourceBEPS(6) / ort->gatherTotalsBEPS(6), 3) +
                                  " (calculated using schedule \"" + GetScheduleName(state, ort->ffSchedIndex(6)) + "\")";
            } else {
                tableBody(1, 6) = "N/A";
            }

            if (!ort->ffSchedUsed(8)) {
                tableBody(1, 7) = RealToStr(ort->sourceFactorDiesel, 3);
            } else if (ort->gatherTotalsBEPS(8) > SmallValue) {
                tableBody(1, 7) = "Effective Factor = " + RealToStr(ort->gatherTotalsBySourceBEPS(8) / ort->gatherTotalsBEPS(8), 3) +
                                  " (calculated using schedule \"" + GetScheduleName(state, ort->ffSchedIndex(8)) + "\")";
            } else {
                tableBody(1, 7) = "N/A";
            }

            if (!ort->ffSchedUsed(9)) {
                tableBody(1, 8) = RealToStr(ort->sourceFactorCoal, 3);
            } else if (ort->gatherTotalsBEPS(9) > SmallValue) {
                tableBody(1, 8) = "Effective Factor = " + RealToStr(ort->gatherTotalsBySourceBEPS(9) / ort->gatherTotalsBEPS(9), 3) +
                                  " (calculated using schedule \"" + GetScheduleName(state, ort->ffSchedIndex(9)) + "\")";
            } else {
                tableBody(1, 8) = "N/A";
            }

            if (!ort->ffSchedUsed(10)) {
                tableBody(1, 9) = RealToStr(ort->sourceFactorFuelOil1, 3);
            } else if (ort->gatherTotalsBEPS(10) > SmallValue) {
                tableBody(1, 9) = "Effective Factor = " + RealToStr(ort->gatherTotalsBySourceBEPS(10) / ort->gatherTotalsBEPS(10), 3) +
                                  " (calculated using schedule \"" + GetScheduleName(state, ort->ffSchedIndex(10)) + "\")";
            } else {
                tableBody(1, 9) = "N/A";
            }

            if (!ort->ffSchedUsed(11)) {
                tableBody(1, 10) = RealToStr(ort->sourceFactorFuelOil2, 3);
            } else if (ort->gatherTotalsBEPS(11) > SmallValue) {
                tableBody(1, 10) = "Effective Factor = " + RealToStr(ort->gatherTotalsBySourceBEPS(11) / ort->gatherTotalsBEPS(11), 3) +
                                   " (calculated using schedule \"" + GetScheduleName(state, ort->ffSchedIndex(11)) + "\")";
            } else {
                tableBody(1, 10) = "N/A";
            }

            if (!ort->ffSchedUsed(12)) {
                tableBody(1, 11) = RealToStr(ort->sourceFactorPropane, 3);
            } else if (ort->gatherTotalsBEPS(12) > SmallValue) {
                tableBody(1, 11) = "Effective Factor = " + RealToStr(ort->gatherTotalsBySourceBEPS(12) / ort->gatherTotalsBEPS(12), 3) +
                                   " (calculated using schedule \"" + GetScheduleName(state, ort->ffSchedIndex(12)) + "\")";
            } else {
                tableBody(1, 11) = "N/A";
            }

            if (!ort->ffSchedUsed(13)) {
                tableBody(1, 12) = RealToStr(ort->sourceFactorOtherFuel1, 3);
            } else if (ort->gatherTotalsBEPS(13) > SmallValue) {
                tableBody(1, 12) = "Effective Factor = " + RealToStr(ort->gatherTotalsBySourceBEPS(13) / ort->gatherTotalsBEPS(13), 3) +
                                   " (calculated using schedule \"" + GetScheduleName(state, ort->ffSchedIndex(13)) + "\")";
            } else {
                tableBody(1, 12) = "N/A";
            }

            if (!ort->ffSchedUsed(14)) {
                tableBody(1, 13) = RealToStr(ort->sourceFactorOtherFuel2, 3);
            } else if (ort->gatherTotalsBEPS(14) > SmallValue) {
                tableBody(1, 13) = "Effective Factor = " + RealToStr(ort->gatherTotalsBySourceBEPS(14) / ort->gatherTotalsBEPS(14), 3) +
                                   " (calculated using schedule \"" + GetScheduleName(state, ort->ffSchedIndex(14)) + "\")";
            } else {
                tableBody(1, 13) = "N/A";
            }

            // heading for the entire sub-table
            if (ort->displayTabularBEPS) {
                WriteSubtitle(state, "Site to Source Energy Conversion Factors");
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(tableBody,
                                                           rowHead,
                                                           columnHead,
                                                           "AnnualBuildingUtilityPerformanceSummary",
                                                           "Entire Facility",
                                                           "Site to Source Energy Conversion Factors");
                }

                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(tableBody,
                                                                                                rowHead,
                                                                                                columnHead,
                                                                                                "Annual Building Utility Performance Summary",
                                                                                                "Entire Facility",
                                                                                                "Site to Source Energy Conversion Factors");
                }
            }

            //---- Building Area Sub-Table
            rowHead.allocate(3);
            columnHead.allocate(1);
            columnWidth.allocate(1);
            columnWidth = 14; // array assignment - same for all columns
            tableBody.allocate(1, 3);

            {
                auto const SELECT_CASE_var(ort->unitsStyle);
                if (SELECT_CASE_var == iUnitsStyle::JtoKWH) {
                    columnHead(1) = "Area [m2]";
                } else if (SELECT_CASE_var == iUnitsStyle::InchPound) {
                    columnHead(1) = "Area [ft2]";
                } else {
                    columnHead(1) = "Area [m2]";
                }
            }

            rowHead(1) = "Total Building Area";
            rowHead(2) = "Net Conditioned Building Area";
            rowHead(3) = "Unconditioned Building Area";

            tableBody = "";
            tableBody(1, 1) = RealToStr(convBldgGrossFloorArea, 2);
            if (ort->unitsStyle == iUnitsStyle::InchPound) {
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedGenData, "Total gross floor area [ft2]", RealToStr(convBldgGrossFloorArea, 2));
            } else {
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedGenData, "Total gross floor area [m2]", RealToStr(convBldgGrossFloorArea, 2));
            }
            tableBody(1, 2) = RealToStr(convBldgCondFloorArea, 2);
            tableBody(1, 3) = RealToStr(convBldgGrossFloorArea - convBldgCondFloorArea, 2);

            // heading for the entire sub-table
            if (ort->displayTabularBEPS) {
                WriteSubtitle(state, "Building Area");
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(
                        tableBody, rowHead, columnHead, "AnnualBuildingUtilityPerformanceSummary", "Entire Facility", "Building Area");
                }

                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                        tableBody, rowHead, columnHead, "Annual Building Utility Performance Summary", "Entire Facility", "Building Area");
                }
            }

            //---- End Use Sub-Table
            rowHead.allocate(16);
            columnHead.allocate(13);
            columnWidth.allocate(13);
            columnWidth = 10; // array assignment - same for all columns
            tableBody.allocate(13, 16);
            for (iResource = 1; iResource <= 13; ++iResource) {
                useVal(iResource, 1) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Heating));
                useVal(iResource, 2) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Cooling));
                useVal(iResource, 3) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::InteriorLights));
                useVal(iResource, 4) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::ExteriorLights));
                useVal(iResource, 5) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::InteriorEquipment));
                useVal(iResource, 6) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::ExteriorEquipment));
                useVal(iResource, 7) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Fans));
                useVal(iResource, 8) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Pumps));
                useVal(iResource, 9) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::HeatRejection));
                useVal(iResource, 10) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Humidification));
                useVal(iResource, 11) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::HeatRecovery));
                useVal(iResource, 12) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::WaterSystem));
                useVal(iResource, 13) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Refrigeration));
                useVal(iResource, 14) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Cogeneration));

                useVal(iResource, 15) = collapsedTotal(iResource); // totals
            }

            rowHead(1) = "Heating";
            rowHead(2) = "Cooling";
            rowHead(3) = "Interior Lighting";
            rowHead(4) = "Exterior Lighting";
            rowHead(5) = "Interior Equipment";
            rowHead(6) = "Exterior Equipment";
            rowHead(7) = "Fans";
            rowHead(8) = "Pumps";
            rowHead(9) = "Heat Rejection";
            rowHead(10) = "Humidification";
            rowHead(11) = "Heat Recovery";
            rowHead(12) = "Water Systems";
            rowHead(13) = "Refrigeration";
            rowHead(14) = "Generators";
            rowHead(15) = "";
            rowHead(16) = "Total End Uses";

            {
                auto const SELECT_CASE_var(ort->unitsStyle);
                if (SELECT_CASE_var == iUnitsStyle::JtoKWH) {
                    columnHead(1) = "Electricity [kWh]";
                    columnHead(2) = "Natural Gas [kWh]";
                    columnHead(3) = "Gasoline [kWh]";
                    columnHead(4) = "Diesel [kWh]";
                    columnHead(5) = "Coal [kWh]";
                    columnHead(6) = "Fuel Oil No 1 [kWh]";
                    columnHead(7) = "Fuel Oil No 2 [kWh]";
                    columnHead(8) = "Propane [kWh]";
                    columnHead(9) = "Other Fuel 1 [kWh]";
                    columnHead(10) = "Other Fuel 2 [kWh]";
                    columnHead(11) = "District Cooling [kWh]";
                    columnHead(12) = "District Heating [kWh]";
                    columnHead(13) = "Water [m3]";
                } else if (SELECT_CASE_var == iUnitsStyle::InchPound) {
                    columnHead(1) = "Electricity [kBtu]";
                    columnHead(2) = "Natural Gas [kBtu]";
                    columnHead(3) = "Gasoline [kBtu]";
                    columnHead(4) = "Diesel [kBtu]";
                    columnHead(5) = "Coal [kBtu]";
                    columnHead(6) = "Fuel Oil No 1 [kBtu]";
                    columnHead(7) = "Fuel Oil No 2 [kBtu]";
                    columnHead(8) = "Propane [kBtu]";
                    columnHead(9) = "Other Fuel 1 [kBtu]";
                    columnHead(10) = "Other Fuel 2 [kBtu]";
                    columnHead(11) = "District Cooling [kBtu]";
                    columnHead(12) = "District Heating [kBtu]";
                    columnHead(13) = "Water [gal]";
                } else {
                    columnHead(1) = "Electricity [GJ]";
                    columnHead(2) = "Natural Gas [GJ]";
                    columnHead(3) = "Gasoline [GJ]";
                    columnHead(4) = "Diesel [GJ]";
                    columnHead(5) = "Coal [GJ]";
                    columnHead(6) = "Fuel Oil No 1 [GJ]";
                    columnHead(7) = "Fuel Oil No 2 [GJ]";
                    columnHead(8) = "Propane [GJ]";
                    columnHead(9) = "Other Fuel 1 [GJ]";
                    columnHead(10) = "Other Fuel 2 [GJ]";
                    columnHead(11) = "District Cooling [GJ]";
                    columnHead(12) = "District Heating [GJ]";
                    columnHead(13) = "Water [m3]";
                }
            }

            tableBody = "";
            for (iResource = 1; iResource <= 13; ++iResource) {
                for (size_t jEndUse = 1; jEndUse <= 14; ++jEndUse) {
                    tableBody(iResource, jEndUse) = RealToStr(useVal(iResource, jEndUse), 2);
                }
                tableBody(iResource, 16) = RealToStr(useVal(iResource, 15), 2);
            }
            // add warning message if end use values do not add up to total
            for (iResource = 1; iResource <= 13; ++iResource) {
                Real64 curTotal = 0.0;
                for (int jUse = 1; jUse <= 14; ++jUse) {
                    curTotal += useVal(iResource, jUse);
                }
                if (std::abs(curTotal - collapsedTotal(iResource)) > (collapsedTotal(iResource) * 0.001)) {
                    ShowWarningError(state, ResourceWarningMessage(columnHead(iResource)));
                }
            }

            unconvert = largeConversionFactor / 1000000000.0; // to avoid double converting, the values for the LEED report should be in GJ
            //  Energy Use Intensities - Electricity
            if (ort->buildingGrossFloorArea > 0) {
                PreDefTableEntry(state,
                    state.dataOutRptPredefined->pdchLeedEuiElec, "Interior Lighting (All)", unconvert * 1000 * useVal(colElectricity, 3) / ort->buildingGrossFloorArea, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEuiElec, "Space Heating", unconvert * 1000 * useVal(colElectricity, 1) / ort->buildingGrossFloorArea, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEuiElec, "Space Cooling", unconvert * 1000 * useVal(colElectricity, 2) / ort->buildingGrossFloorArea, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEuiElec, "Fans (All)", unconvert * 1000 * useVal(colElectricity, 7) / ort->buildingGrossFloorArea, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEuiElec, "Service Water Heating", unconvert * 1000 * useVal(colElectricity, 12) / ort->buildingGrossFloorArea, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEuiElec, "Receptacle Equipment", unconvert * 1000 * useVal(colElectricity, 5) / ort->buildingGrossFloorArea, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEuiElec, "Miscellaneous (All)", unconvert * 1000 * (useVal(colElectricity, 15)) / ort->buildingGrossFloorArea, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEuiElec, "Subtotal", unconvert * 1000 * useVal(colElectricity, 15) / ort->buildingGrossFloorArea, 2);
            }

            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEusTotal, "Electricity", unconvert * useVal(colElectricity, 15), 2);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEusProc, "Electricity", unconvert * (useVal(colElectricity, 5) + useVal(colElectricity, 13)), 2);
            if (useVal(colElectricity, 15) != 0) {
                processFraction = (useVal(colElectricity, 5) + useVal(colElectricity, 13)) / useVal(colElectricity, 15);
                processElecCost = state.dataOutRptPredefined->LEEDelecCostTotal * processFraction;
            } else {
                processElecCost = 0.0;
            }
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEcsProc, "Electricity", processElecCost, 2);
            addFootNoteSubTable(state, state.dataOutRptPredefined->pdstLeedEneCostSum, "Process energy cost based on ratio of process to total energy.");

            //  Energy Use Intensities- Natural Gas
            if (ort->buildingGrossFloorArea > 0) {
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEuiNatG, "Space Heating", unconvert * 1000 * useVal(colGas, 1) / ort->buildingGrossFloorArea, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEuiNatG, "Service Water Heating", unconvert * 1000 * useVal(colGas, 12) / ort->buildingGrossFloorArea, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEuiNatG, "Miscellaneous (All)", unconvert * 1000 * useVal(colGas, 15) / ort->buildingGrossFloorArea, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEuiNatG, "Subtotal", unconvert * 1000 * useVal(colGas, 15) / ort->buildingGrossFloorArea, 2);
            }
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEusTotal, "Natural Gas", unconvert * useVal(colGas, 15), 2);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEusProc, "Natural Gas", unconvert * (useVal(colGas, 5) + useVal(colGas, 13)), 2);
            if (useVal(colGas, 15) != 0) {
                processFraction = (useVal(colGas, 5) + useVal(colGas, 13)) / useVal(colGas, 15);
                processGasCost = state.dataOutRptPredefined->LEEDgasCostTotal * processFraction;
            } else {
                processGasCost = 0.0;
            }
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEcsProc, "Natural Gas", processGasCost, 2);

            //  Energy Use Intensities  - Additional Fuel
            useValColAddFuel15 =
                useVal(3, 15) + useVal(4, 15) + useVal(5, 15) + useVal(6, 15) + useVal(7, 15) + useVal(8, 15) + useVal(9, 15) + useVal(10, 15);
            useValColAddFuel5 =
                useVal(3, 5) + useVal(4, 5) + useVal(5, 5) + useVal(6, 5) + useVal(7, 5) + useVal(8, 5) + useVal(9, 5) + useVal(10, 5);
            useValColAddFuel13 =
                useVal(3, 13) + useVal(4, 13) + useVal(5, 13) + useVal(6, 13) + useVal(7, 13) + useVal(8, 13) + useVal(9, 13) + useVal(10, 13);
            if (ort->buildingGrossFloorArea > 0) {
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEuiOthr, "Miscellaneous", unconvert * 1000 * useValColAddFuel15 / ort->buildingGrossFloorArea, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEuiOthr, "Subtotal", unconvert * 1000 * useValColAddFuel15 / ort->buildingGrossFloorArea, 2);
            }
            PreDefTableEntry(state,
                state.dataOutRptPredefined->pdchLeedEusTotal, "Additional", unconvert * (useValColAddFuel15 + useVal(colPurchCool, 15) + useVal(colPurchHeat, 15)), 2);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEusProc,
                             "Additional",
                             unconvert * (useValColAddFuel5 + useValColAddFuel13 + useVal(colPurchCool, 5) +
                                          useVal(colPurchCool, 13) + useVal(colPurchHeat, 5) + useVal(colPurchHeat, 13)),
                             2);
            if ((useValColAddFuel15 + useVal(colPurchCool, 15) + useVal(colPurchHeat, 15)) > 0.001) {
                processFraction = (useValColAddFuel5 + useValColAddFuel13 + useVal(colPurchCool, 5) + useVal(colPurchCool, 13) +
                                   useVal(colPurchHeat, 5) + useVal(colPurchHeat, 13)) /
                                  (useValColAddFuel15 + useVal(colPurchCool, 15) + useVal(colPurchHeat, 15));
            } else {
                processFraction = 0.0;
            }
            processOthrCost = state.dataOutRptPredefined->LEEDothrCostTotal * processFraction;
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEcsProc, "Additional", processOthrCost, 2);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEcsProc, "Total", processElecCost + processGasCost + processOthrCost, 2);
            // accumulate for percentage table
            leedSiteIntLite = 0.0;
            leedSiteSpHeat = 0.0;
            leedSiteSpCool = 0.0;
            leedSiteFanInt = 0.0;
            leedSiteSrvWatr = 0.0;
            leedSiteRecept = 0.0;
            leedSiteTotal = 0.0;
            for (iResource = 1; iResource <= 12; ++iResource) { // don't bother with water
                leedSiteIntLite += useVal(iResource, 3);
                leedSiteSpHeat += useVal(iResource, 1);
                leedSiteSpCool += useVal(iResource, 2);
                leedSiteFanInt += useVal(iResource, 7);
                leedSiteSrvWatr += useVal(iResource, 12);
                leedSiteRecept += useVal(iResource, 5);
                leedSiteTotal += useVal(iResource, 15);
            }
            if (leedSiteTotal != 0) {
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEupPerc, "Interior Lighting (All)", 100 * leedSiteIntLite / leedSiteTotal, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEupPerc, "Space Heating", 100 * leedSiteSpHeat / leedSiteTotal, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEupPerc, "Space Cooling", 100 * leedSiteSpCool / leedSiteTotal, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEupPerc, "Fans (All)", 100 * leedSiteFanInt / leedSiteTotal, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEupPerc, "Service Water Heating", 100 * leedSiteSrvWatr / leedSiteTotal, 2);
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEupPerc, "Receptacle Equipment", 100 * leedSiteRecept / leedSiteTotal, 2);
                PreDefTableEntry(state,
                    state.dataOutRptPredefined->pdchLeedEupPerc,
                    "Miscellaneous",
                    100 * (leedSiteTotal - (leedSiteIntLite + leedSiteSpHeat + leedSiteSpCool + leedSiteFanInt + leedSiteSrvWatr + leedSiteRecept)) /
                        leedSiteTotal,
                    2);
            }
            // totals across energy source
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEusTotal,
                             "Total",
                             unconvert * (useValColAddFuel15 + useVal(colPurchCool, 15) + useVal(colPurchHeat, 15) +
                                          useVal(colElectricity, 15) + useVal(colGas, 15)),
                             2);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedEusProc,
                             "Total",
                             unconvert * (useValColAddFuel5 + useValColAddFuel13 + useVal(colPurchCool, 5) +
                                          useVal(colPurchCool, 13) + useVal(colPurchHeat, 5) + useVal(colPurchHeat, 13) + useVal(colElectricity, 5) +
                                          useVal(colElectricity, 13) + useVal(colGas, 5) + useVal(colGas, 13)),
                             2);

            footnote = "";
            {
                auto const SELECT_CASE_var(resourcePrimaryHeating);
                if (SELECT_CASE_var == colElectricity) {
                    footnote = "Note: Electricity appears to be the principal heating source based on energy usage.";
                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedGenData, "Principal Heating Source", "Electricity");
                } else if (SELECT_CASE_var == colGas) {
                    footnote = "Note: Natural gas appears to be the principal heating source based on energy usage.";
                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedGenData, "Principal Heating Source", "Natural Gas");
                } else if (SELECT_CASE_var == 3 || SELECT_CASE_var == 4 || SELECT_CASE_var == 5 || SELECT_CASE_var == 6 || SELECT_CASE_var == 7 ||
                           SELECT_CASE_var == 8 || SELECT_CASE_var == 9 || SELECT_CASE_var == 10) {
                    footnote = "Note: Additional fuel appears to be the principal heating source based on energy usage.";
                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedGenData, "Principal Heating Source", "Additional Fuel");
                    // additional fuel  <- gasoline (3) | <- diesel (4) | <- coal (5) | <- Fuel Oil No1 (6) | <- Fuel Oil No2 (7)
                    // <- propane (8) | <- otherfuel1 (9) | <- otherfuel2 (10)
                } else if (SELECT_CASE_var == colPurchHeat) {
                    footnote = "Note: District heat appears to be the principal heating source based on energy usage.";
                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedGenData, "Principal Heating Source", "District Heat");
                }
            }
            // heading for the entire sub-table
            if (ort->displayTabularBEPS) {
                WriteSubtitle(state, "End Uses");
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth, false, footnote);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(
                        tableBody, rowHead, columnHead, "AnnualBuildingUtilityPerformanceSummary", "Entire Facility", "End Uses");
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                        tableBody, rowHead, columnHead, "Annual Building Utility Performance Summary", "Entire Facility", "End Uses");
                }
            }

            //---- End Uses By Subcategory Sub-Table

            // determine if subcategories add up to the total and
            // if not, determine the difference for the 'other' row
            ort->needOtherRowLEED45 = false; // set array to all false assuming no other rows are needed
            for (iResource = 1; iResource <= 13; ++iResource) {
                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    if (state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories > 0) {
                        // set the value to the total for the end use
                        endUseSubOther(iResource, jEndUse) = collapsedEndUse(iResource, jEndUse);
                        // subtract off each sub end use category value
                        for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                            endUseSubOther(iResource, jEndUse) -= collapsedEndUseSub(kEndUseSub, jEndUse, iResource);
                        }
                        // if just a small value remains set it to zero
                        if (std::abs(endUseSubOther(iResource, jEndUse)) > 0.01) {
                            ort->needOtherRowLEED45(jEndUse) = true;
                        } else {
                            endUseSubOther(iResource, jEndUse) = 0.0;
                        }
                    } else {
                        endUseSubOther(iResource, jEndUse) = 0.0;
                    }
                }
            }

            // determine the number of rows needed for sub-table
            numRows = 0;
            for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                if (state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories > 0) {
                    for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                        ++numRows;
                    }
                    // check if an 'other' row is needed
                    if (ort->needOtherRowLEED45(jEndUse)) {
                        ++numRows;
                    }
                } else {
                    ++numRows;
                }
            }

            rowHead.allocate(numRows);
            columnHead.allocate(14);
            columnWidth.allocate(14);
            columnWidth = 10;                 // array assignment - same for all columns
            tableBody.allocate(14, numRows); // TODO: this appears to be (column, row)...
            rowHead = "";
            tableBody = "";

            // Build row head and subcategories columns
            i = 1;
            for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                rowHead(i) = state.dataOutputProcessor->EndUseCategory(jEndUse).DisplayName;
                if (state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories > 0) {
                    for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                        tableBody(1, i) = state.dataOutputProcessor->EndUseCategory(jEndUse).SubcategoryName(kEndUseSub);
                        ++i;
                    }
                    // check if an 'other' row is needed
                    if (ort->needOtherRowLEED45(jEndUse)) {
                        tableBody(1, i) = "Other";
                        ++i;
                    }
                } else {
                    tableBody(1, i) = "General";
                    ++i;
                }
            }

            columnHead(1) = "Subcategory";

            {
                auto const SELECT_CASE_var(ort->unitsStyle);
                if (SELECT_CASE_var == iUnitsStyle::JtoKWH) {
                    columnHead(2) = "Electricity [kWh]";
                    columnHead(3) = "Natural Gas [kWh]";
                    columnHead(4) = "Gasoline [kWh]";
                    columnHead(5) = "Diesel [kWh]";
                    columnHead(6) = "Coal [kWh]";
                    columnHead(7) = "Fuel Oil No 1 [kWh]";
                    columnHead(8) = "Fuel Oil No 2 [kWh]";
                    columnHead(9) = "Propane [kWh]";
                    columnHead(10) = "Other Fuel 1 [kWh]";
                    columnHead(11) = "Other Fuel 2 [kWh]";
                    columnHead(12) = "District Cooling [kWh]";
                    columnHead(13) = "District Heating [kWh]";
                    columnHead(14) = "Water [m3]";
                } else if (SELECT_CASE_var == iUnitsStyle::InchPound) {
                    columnHead(2) = "Electricity [kBtu]";
                    columnHead(3) = "Natural Gas [kBtu]";
                    columnHead(4) = "Gasoline [kBtu]";
                    columnHead(5) = "Diesel [kBtu]";
                    columnHead(6) = "Coal [kBtu]";
                    columnHead(7) = "Fuel Oil No 1 [kBtu]";
                    columnHead(8) = "Fuel Oil No 2 [kBtu]";
                    columnHead(9) = "Propane [kBtu]";
                    columnHead(10) = "Other Fuel 1 [kBtu]";
                    columnHead(11) = "Other Fuel 2 [kBtu]";
                    columnHead(12) = "District Cooling [kBtu]";
                    columnHead(13) = "District Heating [kBtu]";
                    columnHead(14) = "Water [gal]";
                } else {
                    columnHead(2) = "Electricity [GJ]";
                    columnHead(3) = "Natural Gas [GJ]";
                    columnHead(4) = "Gasoline [GJ]";
                    columnHead(5) = "Diesel [GJ]";
                    columnHead(6) = "Coal [GJ]";
                    columnHead(7) = "Fuel Oil No 1 [GJ]";
                    columnHead(8) = "Fuel Oil No 2 [GJ]";
                    columnHead(9) = "Propane [GJ]";
                    columnHead(10) = "Other Fuel 1 [GJ]";
                    columnHead(11) = "Other Fuel 2 [GJ]";
                    columnHead(12) = "District Cooling [GJ]";
                    columnHead(13) = "District Heating [GJ]";
                    columnHead(14) = "Water [m3]";
                }
            }

            for (iResource = 1; iResource <= 13; ++iResource) {
                i = 1;
                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    if (state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories > 0) {
                        for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                            tableBody(iResource + 1, i) = RealToStr(collapsedEndUseSub(kEndUseSub, jEndUse, iResource), 2);
                            ++i;
                        }
                        // put other
                        if (ort->needOtherRowLEED45(jEndUse)) {
                            tableBody(iResource + 1, i) = RealToStr(endUseSubOther(iResource, jEndUse), 2);
                            ++i;
                        }
                    } else {
                        tableBody(iResource + 1, i) = RealToStr(collapsedEndUse(iResource, jEndUse), 2);
                        ++i;
                    }
                }
            }

            // heading for the entire sub-table
            if (ort->displayTabularBEPS) {
                WriteSubtitle(state, "End Uses By Subcategory");
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);

                Array1D_string rowHeadTemp(rowHead);
                // Before outputing to SQL, we forward fill the End use column (rowHead)
                // for better sql queries
                FillRowHead(rowHeadTemp);

                for (int i = 1; i <= numRows; ++i) {
                    rowHeadTemp(i) = rowHeadTemp(i) + ":" + tableBody(1, i);
                }

                // Erase the SubCategory (first column), using slicing
                Array2D_string tableBodyTemp(tableBody({2, _, _}, {_, _, _}));
                Array1D_string columnHeadTemp(columnHead({2, _, _}));

                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(tableBodyTemp,
                                                           rowHeadTemp,
                                                           columnHeadTemp,
                                                           "AnnualBuildingUtilityPerformanceSummary",
                                                           "Entire Facility",
                                                           "End Uses By Subcategory");
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(tableBodyTemp,
                                                                                                rowHeadTemp,
                                                                                                columnHeadTemp,
                                                                                                "Annual Building Utility Performance Summary",
                                                                                                "Entire Facility",
                                                                                                "End Uses By Subcategory");
                }
                rowHeadTemp.deallocate();
                tableBodyTemp.deallocate();
                columnHeadTemp.deallocate();
            }

            // EAp2-4/5. Performance Rating Method Compliance
            // repeat some of the code for the end use subcategory table but only looping over the energy resources and not including water

            Array1D_int resource_entry_map;
            resource_entry_map.allocate(12);
            resource_entry_map(1) = state.dataOutRptPredefined->pdchLeedPerfElEneUse;          // electricity
            resource_entry_map(2) = state.dataOutRptPredefined->pdchLeedPerfGasEneUse;         // natural gas
            resource_entry_map(3) = state.dataOutRptPredefined->pdchLeedPerfGasolineEneUse;    // gasoline
            resource_entry_map(4) = state.dataOutRptPredefined->pdchLeedPerfDieselEneUse;      // diesel
            resource_entry_map(5) = state.dataOutRptPredefined->pdchLeedPerfCoalEneUse;        // coal
            resource_entry_map(6) = state.dataOutRptPredefined->pdchLeedPerfFuelOil1EneUse;    // fuel oil no 1
            resource_entry_map(7) = state.dataOutRptPredefined->pdchLeedPerfFuelOil2EneUse;    // fuel oil no 2
            resource_entry_map(8) = state.dataOutRptPredefined->pdchLeedPerfPropaneEneUse;     // propane
            resource_entry_map(9) = state.dataOutRptPredefined->pdchLeedPerfOtherFuel1EneUse;  // other fuel 1
            resource_entry_map(10) = state.dataOutRptPredefined->pdchLeedPerfOtherFuel2EneUse; // other fuel 2
            resource_entry_map(11) = state.dataOutRptPredefined->pdchLeedPerfDisClEneUse;      // district cooling
            resource_entry_map(12) = state.dataOutRptPredefined->pdchLeedPerfDisHtEneUse;      // district heating

            for (iResource = 1; iResource <= 12; ++iResource) {
                i = 1;
                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    if (state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories > 0) {
                        for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                            PreDefTableEntry(state, resource_entry_map(iResource),
                                             state.dataOutputProcessor->EndUseCategory(jEndUse).DisplayName + " -- " + state.dataOutputProcessor->EndUseCategory(jEndUse).SubcategoryName(kEndUseSub),
                                             unconvert * collapsedEndUseSub(kEndUseSub, jEndUse, iResource));
                            ++i;
                        }
                        // put other
                        if (ort->needOtherRowLEED45(jEndUse)) {
                            PreDefTableEntry(state, resource_entry_map(iResource),
                                             state.dataOutputProcessor->EndUseCategory(jEndUse).DisplayName + " -- Other",
                                             unconvert * endUseSubOther(iResource, jEndUse));
                            ++i;
                        }
                    } else {
                        PreDefTableEntry(state, resource_entry_map(iResource),
                                         state.dataOutputProcessor->EndUseCategory(jEndUse).DisplayName + " -- Not Subdivided",
                                         unconvert * collapsedEndUse(iResource, jEndUse));
                        ++i;
                    }
                }
            }

            //---- Normalized by Conditioned Area Sub-Table
            // Calculations for both normalized tables are first
            rowHead.allocate(4);
            columnHead.allocate(13);
            columnWidth.allocate(13);
            columnWidth = 7; // array assignment - same for all columns
            tableBody.allocate(13, 4);
            for (iResource = 1; iResource <= 13; ++iResource) {
                normalVal(iResource, 1) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::InteriorLights)) +
                                          collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::ExteriorLights)); // Lights     <- InteriorLights | <- ExteriorLights

                normalVal(iResource, 2) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Fans)) + collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Pumps)) +
                                          collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Heating)) + collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Cooling)) +
                                          collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::HeatRejection)) + collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Humidification)) +
                                          collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::WaterSystem)); // HVAC       <- fans | <- pumps | <- heating | <- cooling |
                                                                                         // <- heat rejection | <- humidification | <- water system
                                                                                         // domestic hot water

                normalVal(iResource, 3) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::InteriorEquipment)) + collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::ExteriorEquipment)) +
                                          collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Cogeneration)) + collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::HeatRecovery)) +
                                          collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Refrigeration)); // Other      <- InteriorEquipment | <- ExteriorEquipment |
                                                                                           // <- generator fuel | <- Heat Recovery (parasitics) | <-
                                                                                           // Refrigeration

                normalVal(iResource, 4) = collapsedTotal(iResource); // totals
            }
            // convert the normalized end use values to MJ from GJ if using J
            for (iResource = 1; iResource <= 12; ++iResource) { // not including resource=13 water
                for (size_t jEndUse = 1; jEndUse <= 4; ++jEndUse) {
                    normalVal(iResource, jEndUse) *= kConversionFactor;
                }
            }

            rowHead(1) = "Lighting"; // typo fixed 5-17-04 BTG
            rowHead(2) = "HVAC";
            rowHead(3) = "Other";
            rowHead(4) = "Total";

            {
                auto const SELECT_CASE_var(ort->unitsStyle);
                if (SELECT_CASE_var == iUnitsStyle::JtoKWH) {
                    columnHead(1) = "Electricity Intensity [kWh/m2]";
                    columnHead(2) = "Natural Gas Intensity [kWh/m2]";
                    columnHead(3) = "Gasoline Intensity [kWh/m2]";
                    columnHead(4) = "Diesel Intensity [kWh/m2]";
                    columnHead(5) = "Coal Intensity [kWh/m2]";
                    columnHead(6) = "Fuel Oil No 1 Intensity [kWh/m2]";
                    columnHead(7) = "Fuel Oil No 2 Intensity [kWh/m2]";
                    columnHead(8) = "Propane Intensity [kWh/m2]";
                    columnHead(9) = "Other Fuel 1 Intensity [kWh/m2]";
                    columnHead(10) = "Other Fuel 2 Intensity [kWh/m2]";
                    columnHead(11) = "District Cooling Intensity [kWh/m2]";
                    columnHead(12) = "District Heating Intensity [kWh/m2]";
                    columnHead(13) = "Water Intensity [m3/m2]";
                } else if (SELECT_CASE_var == iUnitsStyle::InchPound) {
                    columnHead(1) = "Electricity Intensity [kBtu/ft2]";
                    columnHead(2) = "Natural Gas Intensity [kBtu/ft2]";
                    columnHead(3) = "Gasoline Intensity [kBtu/ft2]";
                    columnHead(4) = "Diesel Intensity [kBtu/ft2]";
                    columnHead(5) = "Coal Intensity [kBtu/ft2]";
                    columnHead(6) = "Fuel Oil No 1 Intensity [kBtu/ft2]";
                    columnHead(7) = "Fuel Oil No 2 Intensity [kBtu/ft2]";
                    columnHead(8) = "Propane Intensity [kBtu/ft2]";
                    columnHead(9) = "Other Fuel 1 Intensity [kBtu/ft2]";
                    columnHead(10) = "Other Fuel 2 Intensity [kBtu/ft2]";
                    columnHead(11) = "District Cooling Intensity [kBtu/ft2]";
                    columnHead(12) = "District Heating Intensity [kBtu/ft2]";
                    columnHead(13) = "Water Intensity [gal/ft2]";
                } else {
                    columnHead(1) = "Electricity Intensity [MJ/m2]";
                    columnHead(2) = "Natural Gas Intensity [MJ/m2]";
                    columnHead(3) = "Gasoline Intensity [MJ/m2]";
                    columnHead(4) = "Diesel Intensity [MJ/m2]";
                    columnHead(5) = "Coal Intensity [MJ/m2]";
                    columnHead(6) = "Fuel Oil No 1 Intensity [MJ/m2]";
                    columnHead(7) = "Fuel Oil No 2 Intensity [MJ/m2]";
                    columnHead(8) = "Propane Intensity [MJ/m2]";
                    columnHead(9) = "Other Fuel 1 Intensity [MJ/m2]";
                    columnHead(10) = "Other Fuel 2 Intensity [MJ/m2]";
                    columnHead(11) = "District Cooling Intensity [MJ/m2]";
                    columnHead(12) = "District Heating Intensity [MJ/m2]";
                    columnHead(13) = "Water Intensity [m3/m2]";
                }
            }

            WriteTextLine(state, "Normalized Metrics", true);

            // write the conditioned area based table
            tableBody = "";
            if (convBldgCondFloorArea > 0) {
                for (iResource = 1; iResource <= 13; ++iResource) {
                    for (size_t jEndUse = 1; jEndUse <= 4; ++jEndUse) {
                        tableBody(iResource, jEndUse) = RealToStr(normalVal(iResource, jEndUse) / convBldgCondFloorArea, 2);
                    }
                }
            }
            // heading for the entire sub-table
            if (ort->displayTabularBEPS) {
                WriteSubtitle(state, "Utility Use Per Conditioned Floor Area");
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(tableBody,
                                                           rowHead,
                                                           columnHead,
                                                           "AnnualBuildingUtilityPerformanceSummary",
                                                           "Entire Facility",
                                                           "Utility Use Per Conditioned Floor Area");
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(tableBody,
                                                                                                rowHead,
                                                                                                columnHead,
                                                                                                "Annual Building Utility Performance Summary",
                                                                                                "Entire Facility",
                                                                                                "Utility Use Per Conditioned Floor Area");
                }
            }
            //---- Normalized by Total Area Sub-Table
            tableBody = "";
            if (convBldgGrossFloorArea > 0) {
                for (iResource = 1; iResource <= 13; ++iResource) {
                    for (size_t jEndUse = 1; jEndUse <= 4; ++jEndUse) {
                        tableBody(iResource, jEndUse) = RealToStr(normalVal(iResource, jEndUse) / convBldgGrossFloorArea, 2);
                    }
                }
            }
            // heading for the entire sub-table
            if (ort->displayTabularBEPS) {
                WriteSubtitle(state, "Utility Use Per Total Floor Area");
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(tableBody,
                                                           rowHead,
                                                           columnHead,
                                                           "AnnualBuildingUtilityPerformanceSummary",
                                                           "Entire Facility",
                                                           "Utility Use Per Total Floor Area");
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(tableBody,
                                                                                                rowHead,
                                                                                                columnHead,
                                                                                                "Annual Building Utility Performance Summary",
                                                                                                "Entire Facility",
                                                                                                "Utility Use Per Total Floor Area");
                }
            }

            //---- Electric Loads Satisfied Sub-Table
            rowHead.allocate(14);
            columnHead.allocate(2);
            columnWidth.allocate(2);
            columnWidth = 14; // array assignment - same for all columns
            tableBody.allocate(2, 14);

            {
                auto const SELECT_CASE_var(ort->unitsStyle);
                if (SELECT_CASE_var == iUnitsStyle::JtoKWH) {
                    columnHead(1) = "Electricity [kWh]";
                } else if (SELECT_CASE_var == iUnitsStyle::InchPound) {
                    columnHead(1) = "Electricity [kBtu]";
                } else {
                    columnHead(1) = "Electricity [GJ]";
                }
            }
            columnHead(2) = "Percent Electricity [%]";

            rowHead(1) = "Fuel-Fired Power Generation";
            rowHead(2) = "High Temperature Geothermal*";
            rowHead(3) = "Photovoltaic Power";
            rowHead(4) = "Wind Power";
            rowHead(5) = "Power Conversion";
            rowHead(6) = "Net Decrease in On-Site Storage";
            rowHead(7) = "Total On-Site Electric Sources";
            rowHead(8) = "";
            rowHead(9) = "Electricity Coming From Utility";
            rowHead(10) = "Surplus Electricity Going To Utility";
            rowHead(11) = "Net Electricity From Utility";
            rowHead(12) = "";
            rowHead(13) = "Total On-Site and Utility Electric Sources";
            rowHead(14) = "Total Electricity End Uses";

            tableBody = "";

            // show annual values
            unconvert = largeConversionFactor / 1000000000.0; // to avoid double converting, the values for the LEED report should be in GJ

            tableBody(1, 1) = RealToStr(ort->gatherPowerFuelFireGen, 3);
            tableBody(1, 2) = RealToStr(ort->gatherPowerHTGeothermal, 3);
            tableBody(1, 3) = RealToStr(ort->gatherPowerPV, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedRenAnGen, "Photovoltaic", unconvert * ort->gatherPowerPV, 2);
            tableBody(1, 4) = RealToStr(ort->gatherPowerWind, 3);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedRenAnGen, "Wind", unconvert * ort->gatherPowerWind, 2);
            tableBody(1, 5) = RealToStr(ort->gatherPowerConversion, 3);
            tableBody(1, 6) = RealToStr(ort->OverallNetEnergyFromStorage, 3);
            tableBody(1, 7) = RealToStr(ort->gatherElecProduced, 3);
            tableBody(1, 9) = RealToStr(ort->gatherElecPurchased, 3);
            tableBody(1, 10) = RealToStr(ort->gatherElecSurplusSold, 3);
            tableBody(1, 11) = RealToStr(ort->gatherElecPurchased - ort->gatherElecSurplusSold, 3);
            tableBody(1, 13) = RealToStr(ort->gatherElecProduced + (ort->gatherElecPurchased - ort->gatherElecSurplusSold), 3);
            tableBody(1, 14) = RealToStr(collapsedTotal(1), 3);

            // show annual percentages
            if (collapsedTotal(1) > 0) {
                tableBody(2, 1) = RealToStr(100.0 * ort->gatherPowerFuelFireGen / collapsedTotal(1), 2);
                tableBody(2, 2) = RealToStr(100.0 * ort->gatherPowerHTGeothermal / collapsedTotal(1), 2);
                tableBody(2, 3) = RealToStr(100.0 * ort->gatherPowerPV / collapsedTotal(1), 2);
                tableBody(2, 4) = RealToStr(100.0 * ort->gatherPowerWind / collapsedTotal(1), 2);
                tableBody(2, 5) = RealToStr(100.0 * ort->gatherPowerConversion / collapsedTotal(1), 2);
                tableBody(2, 6) = RealToStr(100.0 * ort->OverallNetEnergyFromStorage / collapsedTotal(1), 2);
                tableBody(2, 7) = RealToStr(100.0 * ort->gatherElecProduced / collapsedTotal(1), 2);
                tableBody(2, 9) = RealToStr(100.0 * ort->gatherElecPurchased / collapsedTotal(1), 2);
                tableBody(2, 10) = RealToStr(100.0 * ort->gatherElecSurplusSold / collapsedTotal(1), 2);
                tableBody(2, 11) = RealToStr(100.0 * (ort->gatherElecPurchased - ort->gatherElecSurplusSold) / collapsedTotal(1), 2);
                tableBody(2, 13) = RealToStr(100.0 * (ort->gatherElecProduced + (ort->gatherElecPurchased - ort->gatherElecSurplusSold)) / collapsedTotal(1), 2);
                tableBody(2, 14) = RealToStr(100.0, 2);
            }

            // heading for the entire sub-table
            if (ort->displayTabularBEPS) {
                WriteSubtitle(state, "Electric Loads Satisfied");
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(
                        tableBody, rowHead, columnHead, "AnnualBuildingUtilityPerformanceSummary", "Entire Facility", "Electric Loads Satisfied");
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                        tableBody, rowHead, columnHead, "Annual Building Utility Performance Summary", "Entire Facility", "Electric Loads Satisfied");
                }
            }

            //---- On-Site Thermal Sources Sub-Table
            rowHead.allocate(7);
            columnHead.allocate(2);
            columnWidth.allocate(2);
            columnWidth = 14; // array assignment - same for all columns
            tableBody.allocate(2, 7);

            {
                auto const SELECT_CASE_var(ort->unitsStyle);
                if (SELECT_CASE_var == iUnitsStyle::JtoKWH) {
                    columnHead(1) = "Heat [kWh]";
                } else if (SELECT_CASE_var == iUnitsStyle::InchPound) {
                    columnHead(1) = "Heat [kBtu]";
                } else {
                    columnHead(1) = "Heat [GJ]";
                }
            }

            columnHead(2) = "Percent Heat [%]";

            rowHead(1) = "Water-Side Heat Recovery";
            rowHead(2) = "Air to Air Heat Recovery for Cooling";
            rowHead(3) = "Air to Air Heat Recovery for Heating";
            rowHead(4) = "High-Temperature Geothermal*";
            rowHead(5) = "Solar Water Thermal";
            rowHead(6) = "Solar Air Thermal";
            rowHead(7) = "Total On-Site Thermal Sources";

            tableBody = "";

            // convert to GJ

            ort->gatherWaterHeatRecovery /= largeConversionFactor;
            ort->gatherAirHeatRecoveryCool /= largeConversionFactor;
            ort->gatherAirHeatRecoveryHeat /= largeConversionFactor;
            ort->gatherHeatHTGeothermal /= largeConversionFactor;
            ort->gatherHeatSolarWater /= largeConversionFactor;
            ort->gatherHeatSolarAir /= largeConversionFactor;

            // determine total on site heat
            totalOnsiteHeat = ort->gatherWaterHeatRecovery + ort->gatherAirHeatRecoveryCool + ort->gatherAirHeatRecoveryHeat + ort->gatherHeatHTGeothermal +
                              ort->gatherHeatSolarWater + ort->gatherHeatSolarAir;

            // show annual values
            tableBody(1, 1) = RealToStr(ort->gatherWaterHeatRecovery, 2);
            tableBody(1, 2) = RealToStr(ort->gatherAirHeatRecoveryCool, 2);
            tableBody(1, 3) = RealToStr(ort->gatherAirHeatRecoveryHeat, 2);
            tableBody(1, 4) = RealToStr(ort->gatherHeatHTGeothermal, 2);
            tableBody(1, 5) = RealToStr(ort->gatherHeatSolarWater, 2);
            tableBody(1, 6) = RealToStr(ort->gatherHeatSolarAir, 2);
            tableBody(1, 7) = RealToStr(totalOnsiteHeat, 2);

            if (totalOnsiteHeat > 0) {
                tableBody(2, 1) = RealToStr(100.0 * ort->gatherWaterHeatRecovery / totalOnsiteHeat, 2);
                tableBody(2, 2) = RealToStr(100.0 * ort->gatherAirHeatRecoveryCool / totalOnsiteHeat, 2);
                tableBody(2, 3) = RealToStr(100.0 * ort->gatherAirHeatRecoveryHeat / totalOnsiteHeat, 2);
                tableBody(2, 4) = RealToStr(100.0 * ort->gatherHeatHTGeothermal / totalOnsiteHeat, 2);
                tableBody(2, 5) = RealToStr(100.0 * ort->gatherHeatSolarWater / totalOnsiteHeat, 2);
                tableBody(2, 6) = RealToStr(100.0 * ort->gatherHeatSolarAir / totalOnsiteHeat, 2);
                tableBody(2, 7) = RealToStr(100.0, 2);
            }

            // heading for the entire sub-table
            if (ort->displayTabularBEPS) {
                WriteSubtitle(state, "On-Site Thermal Sources");
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(
                        tableBody, rowHead, columnHead, "AnnualBuildingUtilityPerformanceSummary", "Entire Facility", "On-Site Thermal Sources");
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                        tableBody, rowHead, columnHead, "Annual Building Utility Performance Summary", "Entire Facility", "On-Site Thermal Sources");
                }
            }

            //---- Water Loads Sub-Table
            // As of 12/8/2003 decided to not include this sub-table to wait
            // until water use is implemented in EnergyPlus before displaying
            // the table. Implementing water end-uses makes sense for EnergyPlus
            // but since they are not really implemented as of December 2003 the
            // table would be all zeros.  Recommendation to exclude this table
            // for now made by Glazer and Crawley.
            // Aug 2006, adding table in with implementation of water system, BGriffith
            rowHead.allocate(13);
            columnHead.allocate(2);
            columnWidth.allocate(2);
            columnWidth = 14; // array assignment - same for all columns
            tableBody.allocate(2, 13);
            {
                auto const SELECT_CASE_var(ort->unitsStyle);
                if (SELECT_CASE_var == iUnitsStyle::JtoKWH) {
                    columnHead(1) = "Water [m3]";
                } else if (SELECT_CASE_var == iUnitsStyle::InchPound) {
                    columnHead(1) = "Water [gal]";
                } else {
                    columnHead(1) = "Water [m3]";
                }
            }
            columnHead(2) = "Percent Water [%]";
            rowHead(1) = "Rainwater Collection";
            rowHead(2) = "Condensate Collection";
            rowHead(3) = "Groundwater Well";
            rowHead(4) = "Total On Site Water Sources";
            rowHead(5) = "-";
            rowHead(6) = "Initial Storage";
            rowHead(7) = "Final Storage";
            rowHead(8) = "Change in Storage";
            rowHead(9) = "-";
            rowHead(10) = "Water Supplied by Utility";
            rowHead(11) = "-";
            rowHead(12) = "Total On Site, Change in Storage, and Utility Water Sources";
            rowHead(13) = "Total Water End Uses";
            tableBody = "-";

            totalOnsiteWater = ort->gatherRainWater + ort->gatherCondensate + ort->gatherWellwater;

            //  ! show annual values
            tableBody(1, 1) = RealToStr(ort->gatherRainWater / waterConversionFactor, 2);
            tableBody(1, 2) = RealToStr(ort->gatherCondensate / waterConversionFactor, 2);
            tableBody(1, 3) = RealToStr(ort->gatherWellwater / waterConversionFactor, 2);
            tableBody(1, 4) = RealToStr(totalOnsiteWater / waterConversionFactor, 2);

            if (allocated(state.dataWaterData->WaterStorage)) {
                initialStorage = sum(state.dataWaterData->WaterStorage, &StorageTankDataStruct::InitialVolume);
                finalStorage = sum(state.dataWaterData->WaterStorage, &StorageTankDataStruct::ThisTimeStepVolume);
                StorageChange = initialStorage - finalStorage;
            } else {
                initialStorage = 0.0;
                finalStorage = 0.0;
                StorageChange = 0.0;
            }
            tableBody(1, 6) = RealToStr(initialStorage / waterConversionFactor, 2);
            tableBody(1, 7) = RealToStr(finalStorage / waterConversionFactor, 2);
            tableBody(1, 8) = RealToStr(StorageChange / waterConversionFactor, 2);

            totalWater = totalOnsiteWater + ort->gatherMains + StorageChange;

            tableBody(1, 10) = RealToStr(ort->gatherMains / waterConversionFactor, 2);
            tableBody(1, 12) = RealToStr(totalWater / waterConversionFactor, 2);
            tableBody(1, 13) = RealToStr(ort->gatherWaterEndUseTotal / waterConversionFactor, 2);

            if (ort->gatherWaterEndUseTotal > 0) {
                tableBody(2, 1) = RealToStr(100.0 * ort->gatherRainWater / ort->gatherWaterEndUseTotal, 2);
                tableBody(2, 2) = RealToStr(100.0 * ort->gatherCondensate / ort->gatherWaterEndUseTotal, 2);
                tableBody(2, 3) = RealToStr(100.0 * ort->gatherWellwater / ort->gatherWaterEndUseTotal, 2);
                tableBody(2, 4) = RealToStr(100.0 * totalOnsiteWater / ort->gatherWaterEndUseTotal, 2);
                tableBody(2, 6) = RealToStr(100.0 * initialStorage / ort->gatherWaterEndUseTotal, 2);
                tableBody(2, 7) = RealToStr(100.0 * finalStorage / ort->gatherWaterEndUseTotal, 2);
                tableBody(2, 8) = RealToStr(100.0 * StorageChange / ort->gatherWaterEndUseTotal, 2);

                tableBody(2, 10) = RealToStr(100.0 * ort->gatherMains / ort->gatherWaterEndUseTotal, 2);

                tableBody(2, 12) = RealToStr(100.0 * totalWater / ort->gatherWaterEndUseTotal, 2);
                tableBody(2, 13) = RealToStr(100.0, 2);
            }

            //  ! heading for the entire sub-table
            if (ort->displayTabularBEPS) {
                WriteSubtitle(state, "Water Source Summary");
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(
                        tableBody, rowHead, columnHead, "AnnualBuildingUtilityPerformanceSummary", "Entire Facility", "Water Source Summary");
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                        tableBody, rowHead, columnHead, "Annual Building Utility Performance Summary", "Entire Facility", "Water Source Summary");
                }
            }

            //---- Comfort and Setpoint Not Met Sub-Table
            if (ort->displayTabularBEPS) {
                rowHead.allocate(2);
                columnHead.allocate(1);
                columnWidth.allocate(1);
                columnWidth = 14; // array assignment - same for all columns
                tableBody.allocate(1, 2);

                WriteSubtitle(state, "Setpoint Not Met Criteria");

                curNameWithSIUnits = "Degrees [deltaC]";
                curNameAndUnits = curNameWithSIUnits;
                if (ort->unitsStyle == iUnitsStyle::InchPound) {
                    LookupSItoIP(state, curNameWithSIUnits, indexUnitConv, curNameAndUnits);
                }
                columnHead(1) = curNameAndUnits;

                rowHead(1) = "Tolerance for Zone Heating Setpoint Not Met Time";
                rowHead(2) = "Tolerance for Zone Cooling Setpoint Not Met Time";

                if (ort->unitsStyle != iUnitsStyle::InchPound) {
                    tableBody(1, 1) = RealToStr(std::abs(deviationFromSetPtThresholdHtg), 2);
                    tableBody(1, 2) = RealToStr(deviationFromSetPtThresholdClg, 2);
                } else {
                    tableBody(1, 1) = RealToStr(ConvertIPdelta(state, indexUnitConv, std::abs(deviationFromSetPtThresholdHtg)), 2);
                    tableBody(1, 2) = RealToStr(ConvertIPdelta(state, indexUnitConv, deviationFromSetPtThresholdClg), 2);
                }

                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(
                        tableBody, rowHead, columnHead, "AnnualBuildingUtilityPerformanceSummary", "Entire Facility", "Setpoint Not Met Criteria");
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(tableBody,
                                                                                                rowHead,
                                                                                                columnHead,
                                                                                                "Annual Building Utility Performance Summary",
                                                                                                "Entire Facility",
                                                                                                "Setpoint Not Met Criteria");
                }
            }

            rowHead.allocate(3);
            columnHead.allocate(1);
            columnWidth.allocate(1);
            columnWidth = 14; // array assignment - same for all columns
            tableBody.allocate(1, 3);

            if (ort->displayTabularBEPS) {
                WriteSubtitle(state, "Comfort and Setpoint Not Met Summary");
            }

            columnHead(1) = "Facility [Hours]";

            rowHead(1) = "Time Setpoint Not Met During Occupied Heating";
            rowHead(2) = "Time Setpoint Not Met During Occupied Cooling";
            rowHead(3) = "Time Not Comfortable Based on Simple ASHRAE 55-2004";

            tableBody(1, 1) = RealToStr(state.dataOutRptPredefined->TotalNotMetHeatingOccupiedForABUPS, 2);
            tableBody(1, 2) = RealToStr(state.dataOutRptPredefined->TotalNotMetCoolingOccupiedForABUPS, 2);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedAmData, "Number of hours heating loads not met", RealToStr(state.dataOutRptPredefined->TotalNotMetHeatingOccupiedForABUPS, 2));
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedAmData, "Number of hours cooling loads not met", RealToStr(state.dataOutRptPredefined->TotalNotMetCoolingOccupiedForABUPS, 2));
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedAmData, "Number of hours not met", RealToStr(state.dataOutRptPredefined->TotalNotMetOccupiedForABUPS, 2));
            tableBody(1, 3) = RealToStr(state.dataOutRptPredefined->TotalTimeNotSimpleASH55EitherForABUPS, 2);

            if (ort->displayTabularBEPS) {
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(tableBody,
                                                           rowHead,
                                                           columnHead,
                                                           "AnnualBuildingUtilityPerformanceSummary",
                                                           "Entire Facility",
                                                           "Comfort and Setpoint Not Met Summary");
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(tableBody,
                                                                                                rowHead,
                                                                                                columnHead,
                                                                                                "Annual Building Utility Performance Summary",
                                                                                                "Entire Facility",
                                                                                                "Comfort and Setpoint Not Met Summary");
                }
            }

            //---- Control Summary Sub-Table

            //---- End Notes
            if (ort->displayTabularBEPS) {
                WriteTextLine(state, "Note 1: An asterisk (*) indicates that the feature is not yet implemented.");
            }
            // CALL WriteTextLine('Note 2: The source energy conversion factors used are: ')
            // CALL WriteTextLine('        1.05 for all fuels, 1 for district, and 3 for electricity.')
        }
    }

    std::string ResourceWarningMessage(std::string resource)
    {
        return "In the Annual Building Utility Performance Summary Report the total row does not match the sum of the column for: " + resource;
    }

    Real64 WaterConversionFunct(Real64 WaterTotal, Real64 ConversionFactor)
    {
        return WaterTotal / ConversionFactor;
    }

    void WriteSourceEnergyEndUseSummary(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Mangesh Basarkar
        //       DATE WRITTEN   September 2011
        //       MODIFIED       March 2020, Dareum Nam; Disaggregated "Additional Fuel"
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Take the gathered total and end use source energy meter data and structure
        //   the results into a tabular report for output.

        // METHODOLOGY EMPLOYED:
        //   Create arrays for the call to WriteTable and then call it.

        // all arrays are in the format: (row, column)
        Array1D_string columnHead;
        Array1D_int columnWidth;
        Array1D_string rowHead;
        Array2D_string tableBody;

        // all arrays are in the format: (row, columnm)
        Array2D<Real64> useVal(13, 15);
        Array1D<Real64> collapsedTotal(13);
        Array2D<Real64> collapsedEndUse(13, DataGlobalConstants::iEndUse.size());
        Array3D<Real64> collapsedEndUseSub(state.dataOutputProcessor->MaxNumSubcategories, DataGlobalConstants::iEndUse.size(), 13);
        int iResource;
        Real64 largeConversionFactor;
        Real64 areaConversionFactor;
        auto &ort(state.dataOutRptTab);

        if (ort->displaySourceEnergyEndUseSummary) {
            // show the headers of the report
            WriteReportHeaders(state, "Source Energy End Use Components Summary", "Entire Facility", OutputProcessor::StoreType::Averaged);
            // show the number of hours that the table applies to
            WriteTextLine(state, "Values gathered over " + RealToStr(ort->gatherElapsedTimeBEPS, 2) + " hours", true);
            if (ort->gatherElapsedTimeBEPS < 8759.0) { // might not add up to 8760 exactly but can't be more than 1 hour diff.
                WriteTextLine(state, "WARNING: THE REPORT DOES NOT REPRESENT A FULL ANNUAL SIMULATION.", true);
            }
            WriteTextLine(state, "", true);
            // determine building floor areas
            DetermineBuildingFloorArea(state);
            // collapse the gatherEndUseBEPS array to the resource groups displayed
            for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                collapsedEndUse(1, jEndUse) = ort->gatherEndUseBySourceBEPS(1, jEndUse);   // electricity
                collapsedEndUse(2, jEndUse) = ort->gatherEndUseBySourceBEPS(2, jEndUse);   // natural gas
                collapsedEndUse(3, jEndUse) = ort->gatherEndUseBySourceBEPS(6, jEndUse);   // gasoline
                collapsedEndUse(4, jEndUse) = ort->gatherEndUseBySourceBEPS(8, jEndUse);   // diesel
                collapsedEndUse(5, jEndUse) = ort->gatherEndUseBySourceBEPS(9, jEndUse);   // coal
                collapsedEndUse(6, jEndUse) = ort->gatherEndUseBySourceBEPS(10, jEndUse);  // Fuel Oil No1
                collapsedEndUse(7, jEndUse) = ort->gatherEndUseBySourceBEPS(11, jEndUse);  // Fuel Oil No2
                collapsedEndUse(8, jEndUse) = ort->gatherEndUseBySourceBEPS(12, jEndUse);  // propane
                collapsedEndUse(9, jEndUse) = ort->gatherEndUseBySourceBEPS(13, jEndUse);  // otherfuel1
                collapsedEndUse(10, jEndUse) = ort->gatherEndUseBySourceBEPS(14, jEndUse); // otherfuel2
                collapsedEndUse(11, jEndUse) = ort->gatherEndUseBySourceBEPS(3, jEndUse);  // district cooling <- purchased cooling
                collapsedEndUse(12, jEndUse) =
                    ort->gatherEndUseBySourceBEPS(4, jEndUse) + ort->gatherEndUseBySourceBEPS(5, jEndUse); // district heating <- purchased heating | <- steam
                collapsedEndUse(13, jEndUse) = ort->gatherEndUseBySourceBEPS(7, jEndUse);             // water
            }
            // repeat with totals
            collapsedTotal(1) = ort->gatherTotalsBySourceBEPS(1);                                // electricity
            collapsedTotal(2) = ort->gatherTotalsBySourceBEPS(2);                                // natural gas
            collapsedTotal(3) = ort->gatherTotalsBySourceBEPS(6);                                // gasoline
            collapsedTotal(4) = ort->gatherTotalsBySourceBEPS(8);                                // diesel
            collapsedTotal(5) = ort->gatherTotalsBySourceBEPS(9);                                // coal
            collapsedTotal(6) = ort->gatherTotalsBySourceBEPS(10);                               // Fuel Oil No1
            collapsedTotal(7) = ort->gatherTotalsBySourceBEPS(11);                               // Fuel Oil No2
            collapsedTotal(8) = ort->gatherTotalsBySourceBEPS(12);                               // propane
            collapsedTotal(9) = ort->gatherTotalsBySourceBEPS(13);                               // otherfuel1
            collapsedTotal(10) = ort->gatherTotalsBySourceBEPS(14);                              // otherfuel2
            collapsedTotal(11) = ort->gatherTotalsBySourceBEPS(3);                               // district cooling <- purchased cooling
            collapsedTotal(12) = ort->gatherTotalsBySourceBEPS(4) + ort->gatherTotalsBySourceBEPS(5); // district heating <- purchased heating | <- steam
            collapsedTotal(13) = ort->gatherTotalsBySourceBEPS(7);                               // water

            // unit conversion - all values are used as divisors

            {
                auto const SELECT_CASE_var(ort->unitsStyle);
                if (SELECT_CASE_var == iUnitsStyle::JtoKWH) {
                    largeConversionFactor = 3600000.0;
                    areaConversionFactor = 1.0;
                } else if (SELECT_CASE_var == iUnitsStyle::InchPound) {
                    largeConversionFactor = getSpecificUnitDivider(state, "J", "kBtu"); // 1054351.84 J to kBtu
                    areaConversionFactor = getSpecificUnitDivider(state, "m2", "ft2");  // 0.092893973 m2 to ft2
                } else {
                    largeConversionFactor = 1000000.0; // to MJ
                    areaConversionFactor = 1.0;
                }
            }

            // convert units into MJ (divide by 1,000,000) if J otherwise kWh
            for (iResource = 1; iResource <= 12; ++iResource) { // don't do water
                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    collapsedEndUse(iResource, jEndUse) /= largeConversionFactor;
                }
                collapsedTotal(iResource) /= largeConversionFactor;
            }

            rowHead.allocate(16);
            columnHead.allocate(12);
            columnWidth.allocate(12);
            columnWidth = 10; // array assignment - same for all columns
            tableBody.allocate(12, 16);
            for (iResource = 1; iResource <= 13; ++iResource) {
                useVal(iResource, 1) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Heating));
                useVal(iResource, 2) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Cooling));
                useVal(iResource, 3) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::InteriorLights));
                useVal(iResource, 4) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::ExteriorLights));
                useVal(iResource, 5) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::InteriorEquipment));
                useVal(iResource, 6) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::ExteriorEquipment));
                useVal(iResource, 7) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Fans));
                useVal(iResource, 8) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Pumps));
                useVal(iResource, 9) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::HeatRejection));
                useVal(iResource, 10) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Humidification));
                useVal(iResource, 11) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::HeatRecovery));
                useVal(iResource, 12) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::WaterSystem));
                useVal(iResource, 13) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Refrigeration));
                useVal(iResource, 14) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Cogeneration));

                useVal(iResource, 15) = collapsedTotal(iResource); // totals
            }

            rowHead(1) = "Heating";
            rowHead(2) = "Cooling";
            rowHead(3) = "Interior Lighting";
            rowHead(4) = "Exterior Lighting";
            rowHead(5) = "Interior Equipment";
            rowHead(6) = "Exterior Equipment";
            rowHead(7) = "Fans";
            rowHead(8) = "Pumps";
            rowHead(9) = "Heat Rejection";
            rowHead(10) = "Humidification";
            rowHead(11) = "Heat Recovery";
            rowHead(12) = "Water Systems";
            rowHead(13) = "Refrigeration";
            rowHead(14) = "Generators";
            rowHead(15) = "";
            rowHead(16) = "Total Source Energy End Use Components";

            largeConversionFactor = 1.0;

            {
                auto const SELECT_CASE_var(ort->unitsStyle);
                if (SELECT_CASE_var == iUnitsStyle::JtoKWH) {
                    columnHead(1) = "Source Electricity [kWh]";
                    columnHead(2) = "Source Natural Gas [kWh]";
                    columnHead(3) = "Source Gasoline [kWh]";
                    columnHead(4) = "Source Diesel [kWh]";
                    columnHead(5) = "Source Coal [kWh]";
                    columnHead(6) = "Source Fuel Oil No 1 [kWh]";
                    columnHead(7) = "Source Fuel Oil No 2 [kWh]";
                    columnHead(8) = "Source Propane [kWh]";
                    columnHead(9) = "Source Other Fuel 1 [kWh]";
                    columnHead(10) = "Source Other Fuel 2 [kWh]";
                    columnHead(11) = "Source District Cooling [kWh]";
                    columnHead(12) = "Source District Heating [kWh]";
                } else if (SELECT_CASE_var == iUnitsStyle::InchPound) {
                    columnHead(1) = "Source Electricity [kBtu]";
                    columnHead(2) = "Source Natural Gas [kBtu]";
                    columnHead(3) = "Source Gasoline [kBtu]";
                    columnHead(4) = "Source Diesel [kBtu]";
                    columnHead(5) = "Source Coal [kBtu]";
                    columnHead(6) = "Source Fuel Oil No 1 [kBtu]";
                    columnHead(7) = "Source Fuel Oil No 2 [kBtu]";
                    columnHead(8) = "Source Propane [kBtu]";
                    columnHead(9) = "Source Other Fuel 1 [kBtu]";
                    columnHead(10) = "Source Other Fuel 2 [kBtu]";
                    columnHead(11) = "Source District Cooling [kBtu]";
                    columnHead(12) = "Source District Heating [kBtu]";
                } else {
                    columnHead(1) = "Source Electricity [GJ]";
                    columnHead(2) = "Source Natural Gas [GJ]";
                    columnHead(3) = "Source Gasoline [GJ]";
                    columnHead(4) = "Source Diesel [GJ]";
                    columnHead(5) = "Source Coal [GJ]";
                    columnHead(6) = "Source Fuel Oil No 1 [GJ]";
                    columnHead(7) = "Source Fuel Oil No 2 [GJ]";
                    columnHead(8) = "Source Propane [GJ]";
                    columnHead(9) = "Source Other Fuel 1 [GJ]";
                    columnHead(10) = "Source Other Fuel 2 [GJ]";
                    columnHead(11) = "Source District Cooling [GJ]";
                    columnHead(12) = "Source District Heating [GJ]";
                    largeConversionFactor = 1000.0; // for converting MJ to GJ
                }
            }

            //---- End Uses by Source Energy Sub-Table

            tableBody = "";
            for (iResource = 1; iResource <= 12; ++iResource) {
                for (size_t jEndUse = 1; jEndUse <= 14; ++jEndUse) {
                    tableBody(iResource, jEndUse) = RealToStr(useVal(iResource, jEndUse) / largeConversionFactor, 2);
                }
                tableBody(iResource, 16) = RealToStr(useVal(iResource, 15) / largeConversionFactor, 2);
            }

            // heading for the entire sub-table
            WriteSubtitle(state, "Source Energy End Use Components Summary");
            WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
            if (sqlite) {
                sqlite->createSQLiteTabularDataRecords(tableBody,
                                                       rowHead,
                                                       columnHead,
                                                       "SourceEnergyEndUseComponentsSummary",
                                                       "Entire Facility",
                                                       "Source Energy End Use Components Summary");
            }
            if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(tableBody,
                                                                                            rowHead,
                                                                                            columnHead,
                                                                                            "Source Energy End Use Components Summary",
                                                                                            "Entire Facility",
                                                                                            "Source Energy End Use Components Summary");
            }

            // Normalized by Area tables

            {
                auto const SELECT_CASE_var(ort->unitsStyle);
                if (SELECT_CASE_var == iUnitsStyle::JtoKWH) {
                    columnHead(1) = "Source Electricity [kWh/m2]";
                    columnHead(2) = "Source Natural Gas [kWh/m2]";
                    columnHead(3) = "Source Gasoline [kWh/m2]";
                    columnHead(4) = "Source Diesel [kWh/m2]";
                    columnHead(5) = "Source Coal [kWh/m2]";
                    columnHead(6) = "Source Fuel Oil No 1 [kWh/m2]";
                    columnHead(7) = "Source Fuel Oil No 2 [kWh/m2]";
                    columnHead(8) = "Source Propane [kWh/m2]";
                    columnHead(9) = "Source Other Fuel 1 [kWh/m2]";
                    columnHead(10) = "Source Other Fuel 2 [kWh/m2]";
                    columnHead(11) = "Source District Cooling [kWh/m2]";
                    columnHead(12) = "Source District Heating [kWh/m2]";
                } else if (SELECT_CASE_var == iUnitsStyle::InchPound) {
                    columnHead(1) = "Source Electricity [kBtu/ft2]";
                    columnHead(2) = "Source Natural Gas [kBtu/ft2]";
                    columnHead(3) = "Source Gasoline [kBtu/ft2]";
                    columnHead(4) = "Source Diesel [kBtu/ft2]";
                    columnHead(5) = "Source Coal [kBtu/ft2]";
                    columnHead(6) = "Source Fuel Oil No 1 [kBtu/ft2]";
                    columnHead(7) = "Source Fuel Oil No 2 [kBtu/ft2]";
                    columnHead(8) = "Source Propane [kBtu/ft2]";
                    columnHead(9) = "Source Other Fuel 1 [kBtu/ft2]";
                    columnHead(10) = "Source Other Fuel 2 [kBtu/ft2]";
                    columnHead(11) = "Source District Cooling [kBtu/ft2]";
                    columnHead(12) = "Source District Heating [kBtu/ft2]";
                } else {
                    columnHead(1) = "Source Electricity [MJ/m2]";
                    columnHead(2) = "Source Natural Gas [MJ/m2]";
                    columnHead(3) = "Source Gasoline [MJ/m2]";
                    columnHead(4) = "Source Diesel [MJ/m2]";
                    columnHead(5) = "Source Coal [MJ/m2]";
                    columnHead(6) = "Source Fuel Oil No 1 [MJ/m2]";
                    columnHead(7) = "Source Fuel Oil No 2 [MJ/m2]";
                    columnHead(8) = "Source Propane [MJ/m2]";
                    columnHead(9) = "Source Other Fuel 1 [MJ/m2]";
                    columnHead(10) = "Source Other Fuel 2 [MJ/m2]";
                    columnHead(11) = "Source District Cooling [MJ/m2]";
                    columnHead(12) = "Source District Heating [MJ/m2]";
                }
            }

            //---- Normalized by Conditioned Area Sub-Table
            {
                tableBody = "";
                // convert floor area
                Real64 convBldgCondFloorArea = ort->buildingConditionedFloorArea / areaConversionFactor;
                if (convBldgCondFloorArea > 0) {
                    for (iResource = 1; iResource <= 12; ++iResource) {
                        for (size_t jEndUse = 1; jEndUse <= 14; ++jEndUse) {
                            tableBody(iResource, jEndUse) = RealToStr(useVal(iResource, jEndUse) / convBldgCondFloorArea, 2);
                        }
                        tableBody(iResource, 16) = RealToStr(useVal(iResource, 15) / convBldgCondFloorArea, 2);
                    }
                }

                WriteTextLine(state, "Normalized Metrics", true);

                // heading for the entire sub-table
                WriteSubtitle(state, "Source Energy End Use Components Per Conditioned Floor Area");
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(tableBody,
                                                           rowHead,
                                                           columnHead,
                                                           "SourceEnergyEndUseComponentsSummary",
                                                           "Entire Facility",
                                                           "Source Energy End Use Component Per Conditioned Floor Area");
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                        tableBody,
                        rowHead,
                        columnHead,
                        "Source Energy End Use Components Summary",
                        "Entire Facility",
                        "Source Energy End Use Component Per Conditioned Floor Area");
                }
            } // End of Normalized by Conditioned Area

            //---- Normalized by Total Area Sub-Table
            {
                tableBody = "";
                Real64 convBldgGrossFloorArea = ort->buildingGrossFloorArea / areaConversionFactor;

                if (convBldgGrossFloorArea > 0) {
                    for (iResource = 1; iResource <= 12; ++iResource) {
                        for (size_t jEndUse = 1; jEndUse <= 14; ++jEndUse) {
                            tableBody(iResource, jEndUse) = RealToStr(useVal(iResource, jEndUse) / convBldgGrossFloorArea, 2);
                        }
                        tableBody(iResource, 16) = RealToStr(useVal(iResource, 15) / convBldgGrossFloorArea, 2);
                    }
                }

                // heading for the entire sub-table
                WriteSubtitle(state, "Source Energy End Use Components Per Total Floor Area");
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(tableBody,
                                                           rowHead,
                                                           columnHead,
                                                           "SourceEnergyEndUseComponentsSummary",
                                                           "Entire Facility",
                                                           "Source Energy End Use Components Per Total Floor Area");
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(tableBody,
                                                                                                rowHead,
                                                                                                columnHead,
                                                                                                "Source Energy End Use Components Summary",
                                                                                                "Entire Facility",
                                                                                                "Source Energy End Use Components Per Total Floor Area");
                }
            } // End of Normalized by Total Area

        } // end if displaySourceEnergyEndUseSummary
    }

    void WriteDemandEndUseSummary(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   January 2009
        //       MODIFIED       January 2010, Kyle Benne; Added SQLite output
        //                      March 2020, Dareum Nam; Disaggregated "Additional Fuel"
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Take the gathered total and enduse meter data and structure
        //   the results into a tabular report for output.

        // METHODOLOGY EMPLOYED:
        //   Create arrays for the call to WriteTable and then call it.
        //   This report actually consists of many sub-tables each with
        //   its own call to WriteTable.

        // all arrays are in the format: (row, column)
        Array1D_string columnHead;
        Array1D_int columnWidth;
        Array1D_string rowHead;
        Array2D_string tableBody;

        // all arrays are in the format: (row, column)
        Array2D<Real64> useVal(13, 15);
        Array1D<Real64> collapsedTotal(13);
        Array2D<Real64> collapsedEndUse(13, DataGlobalConstants::iEndUse.size());
        Array2D<Real64> collapsedIndEndUse(13, DataGlobalConstants::iEndUse.size());
        Array1D_int collapsedTimeStep(13);
        Array3D<Real64> collapsedEndUseSub(state.dataOutputProcessor->MaxNumSubcategories, DataGlobalConstants::iEndUse.size(), 13);
        Array3D<Real64> collapsedIndEndUseSub(state.dataOutputProcessor->MaxNumSubcategories, DataGlobalConstants::iEndUse.size(), 13);
        Array2D<Real64> endUseSubOther(13, DataGlobalConstants::iEndUse.size());
        int iResource;
        int kEndUseSub;
        int i;
        int numRows;
        static std::string footnote;
        int distrHeatSelected;
        bool bothDistrHeatNonZero;
        Real64 powerConversion;
        Real64 flowConversion;

        Real64 unconvert;
        std::string subCatName;
        auto &ort(state.dataOutRptTab);

        if (ort->displayDemandEndUse) {

            iUnitsStyle unitsStyle_temp = ort->unitsStyle;
            bool produceTabular = true;
            bool produceSQLite = false;

            for (int iUnitSystem = 0; iUnitSystem <= 1; iUnitSystem++) {
                
                if (iUnitSystem == 0) {
                    produceTabular = true;
                    if (ort->unitsStyle_SQLite == ort->unitsStyle) {
                        produceSQLite = true;
                    } else {
                        produceSQLite = false;
                    }
                    unitsStyle_temp = ort->unitsStyle;
                } else { // iUnitSystem == 1
                    if (ort->unitsStyle_SQLite == ort->unitsStyle) break;
                    produceTabular = false;
                    produceSQLite = true;
                    unitsStyle_temp = ort->unitsStyle_SQLite;
                }

                // show the headers of the report
                WriteReportHeaders(state, "Demand End Use Components Summary", "Entire Facility", OutputProcessor::StoreType::Averaged);
                // totals - select which additional fuel to display and which other district heating
                collapsedTotal = 0.0;
                collapsedTotal(1) = ort->gatherDemandTotal(1); // electricity
                collapsedTimeStep(1) = ort->gatherDemandTimeStamp(1);
                collapsedTotal(2) = ort->gatherDemandTotal(2); // natural gas
                collapsedTimeStep(2) = ort->gatherDemandTimeStamp(2);
                collapsedTotal(3) = ort->gatherDemandTotal(6); // gasoline
                collapsedTimeStep(3) = ort->gatherDemandTimeStamp(6);
                collapsedTotal(4) = ort->gatherDemandTotal(8); // diesel
                collapsedTimeStep(4) = ort->gatherDemandTimeStamp(8);
                collapsedTotal(5) = ort->gatherDemandTotal(9); // coal
                collapsedTimeStep(5) = ort->gatherDemandTimeStamp(9);
                collapsedTotal(6) = ort->gatherDemandTotal(10); // fuel oil no 1
                collapsedTimeStep(6) = ort->gatherDemandTimeStamp(10);
                collapsedTotal(7) = ort->gatherDemandTotal(11); // fuel oil no 2
                collapsedTimeStep(7) = ort->gatherDemandTimeStamp(11);
                collapsedTotal(8) = ort->gatherDemandTotal(12); // propane
                collapsedTimeStep(8) = ort->gatherDemandTimeStamp(12);
                collapsedTotal(9) = ort->gatherDemandTotal(13); // other fuel 1
                collapsedTimeStep(9) = ort->gatherDemandTimeStamp(13);
                collapsedTotal(10) = ort->gatherDemandTotal(14); // other fuel 2
                collapsedTimeStep(10) = ort->gatherDemandTimeStamp(14);
                collapsedTotal(11) = ort->gatherDemandTotal(3); // district cooling <- purchased cooling
                collapsedTimeStep(11) = ort->gatherDemandTimeStamp(3);
                collapsedTotal(13) = ort->gatherDemandTotal(7); // water
                collapsedTimeStep(13) = ort->gatherDemandTimeStamp(7);

                // set flag if both puchased heating and steam both have positive demand
                bothDistrHeatNonZero = (ort->gatherDemandTotal(4) > 0.0) && (ort->gatherDemandTotal(5) > 0.0);
                // select the district heating source that has a larger demand
                if (ort->gatherDemandTotal(4) > ort->gatherDemandTotal(5)) {
                    distrHeatSelected = 4; // purchased heating
                    if (bothDistrHeatNonZero) {
                        footnote += " Steam has non-zero demand but is not shown on this report.";
                    }
                } else {
                    distrHeatSelected = 5; // steam
                    if (bothDistrHeatNonZero) {
                        footnote += " District heating has non-zero demand but is not shown on this report.";
                    }
                }
                // set the time of peak demand and total demand for the purchased heating/steam
                collapsedTimeStep(12) = ort->gatherDemandTimeStamp(distrHeatSelected);
                collapsedTotal(12) = ort->gatherDemandTotal(distrHeatSelected);

                // establish unit conversion factors
                if (unitsStyle_temp == iUnitsStyle::InchPound) {
                    powerConversion = getSpecificUnitMultiplier(state, "W", "kBtuh");
                    flowConversion = getSpecificUnitMultiplier(state, "m3/s", "gal/min");
                } else {
                    powerConversion = 1.0;
                    flowConversion = 1.0;
                }

                // collapse the gatherEndUseBEPS array to the resource groups displayed
                collapsedEndUse = 0.0;
                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    collapsedEndUse(1, jEndUse) = ort->gatherDemandEndUse(1, jEndUse) * powerConversion;                  // electricity
                    collapsedEndUse(2, jEndUse) = ort->gatherDemandEndUse(2, jEndUse) * powerConversion;                  // natural gas
                    collapsedEndUse(3, jEndUse) = ort->gatherDemandEndUse(6, jEndUse) * powerConversion;                  // gasoline
                    collapsedEndUse(4, jEndUse) = ort->gatherDemandEndUse(8, jEndUse) * powerConversion;                  // diesel
                    collapsedEndUse(5, jEndUse) = ort->gatherDemandEndUse(9, jEndUse) * powerConversion;                  // coal
                    collapsedEndUse(6, jEndUse) = ort->gatherDemandEndUse(10, jEndUse) * powerConversion;                 // fuel oil no 1
                    collapsedEndUse(7, jEndUse) = ort->gatherDemandEndUse(11, jEndUse) * powerConversion;                 // fuel oil no 2
                    collapsedEndUse(8, jEndUse) = ort->gatherDemandEndUse(12, jEndUse) * powerConversion;                 // propane
                    collapsedEndUse(9, jEndUse) = ort->gatherDemandEndUse(13, jEndUse) * powerConversion;                 // otherfuel1
                    collapsedEndUse(10, jEndUse) = ort->gatherDemandEndUse(14, jEndUse) * powerConversion;                // otherfuel2
                    collapsedEndUse(11, jEndUse) = ort->gatherDemandEndUse(3, jEndUse) * powerConversion;                 // purchased cooling
                    collapsedEndUse(12, jEndUse) = ort->gatherDemandEndUse(distrHeatSelected, jEndUse) * powerConversion; // district heating
                    collapsedEndUse(13, jEndUse) = ort->gatherDemandEndUse(7, jEndUse) * flowConversion;                  // water
                }
                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                        collapsedEndUseSub(kEndUseSub, jEndUse, 1) =
                            ort->gatherDemandEndUseSub(kEndUseSub, jEndUse, 1) * powerConversion; // electricity
                        collapsedEndUseSub(kEndUseSub, jEndUse, 2) =
                            ort->gatherDemandEndUseSub(kEndUseSub, jEndUse, 2) * powerConversion; // natural gas
                        collapsedEndUseSub(kEndUseSub, jEndUse, 3) = ort->gatherDemandEndUseSub(kEndUseSub, jEndUse, 6) * powerConversion; // gasoline
                        collapsedEndUseSub(kEndUseSub, jEndUse, 4) = ort->gatherDemandEndUseSub(kEndUseSub, jEndUse, 8) * powerConversion; // diesel
                        collapsedEndUseSub(kEndUseSub, jEndUse, 5) = ort->gatherDemandEndUseSub(kEndUseSub, jEndUse, 9) * powerConversion; // coal
                        collapsedEndUseSub(kEndUseSub, jEndUse, 6) =
                            ort->gatherDemandEndUseSub(kEndUseSub, jEndUse, 10) * powerConversion; // fuel oil no 1
                        collapsedEndUseSub(kEndUseSub, jEndUse, 7) =
                            ort->gatherDemandEndUseSub(kEndUseSub, jEndUse, 11) * powerConversion; // fuel oil no 2
                        collapsedEndUseSub(kEndUseSub, jEndUse, 8) = ort->gatherDemandEndUseSub(kEndUseSub, jEndUse, 12) * powerConversion; // propane
                        collapsedEndUseSub(kEndUseSub, jEndUse, 9) =
                            ort->gatherDemandEndUseSub(kEndUseSub, jEndUse, 13) * powerConversion; // otherfuel1
                        collapsedEndUseSub(kEndUseSub, jEndUse, 10) =
                            ort->gatherDemandEndUseSub(kEndUseSub, jEndUse, 14) * powerConversion; // otherfuel2
                        collapsedEndUseSub(kEndUseSub, jEndUse, 11) =
                            ort->gatherDemandEndUseSub(kEndUseSub, jEndUse, 3) * powerConversion; // purch cooling
                        collapsedEndUseSub(kEndUseSub, jEndUse, 12) =
                            ort->gatherDemandEndUseSub(kEndUseSub, jEndUse, distrHeatSelected) * powerConversion; // district heating
                        collapsedEndUseSub(kEndUseSub, jEndUse, 13) = ort->gatherDemandEndUseSub(kEndUseSub, jEndUse, 7) * flowConversion; // water
                    }
                }
                // collapse the individual peaks for the end use subcategories for the LEED report
                // collapse the gatherEndUseBEPS array to the resource groups displayed
                // no unit conversion, it is done at the reporting stage if necessary
                collapsedIndEndUse = 0.0;
                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    collapsedIndEndUse(1, jEndUse) = ort->gatherDemandIndEndUse(1, jEndUse);                  // electricity
                    collapsedIndEndUse(2, jEndUse) = ort->gatherDemandIndEndUse(2, jEndUse);                  // natural gas
                    collapsedIndEndUse(3, jEndUse) = ort->gatherDemandIndEndUse(6, jEndUse);                  // gasoline
                    collapsedIndEndUse(4, jEndUse) = ort->gatherDemandIndEndUse(8, jEndUse);                  // diesel
                    collapsedIndEndUse(5, jEndUse) = ort->gatherDemandIndEndUse(9, jEndUse);                  // coal
                    collapsedIndEndUse(6, jEndUse) = ort->gatherDemandIndEndUse(10, jEndUse);                 // fuel oil no 1
                    collapsedIndEndUse(7, jEndUse) = ort->gatherDemandIndEndUse(11, jEndUse);                 // fuel oil no 2
                    collapsedIndEndUse(8, jEndUse) = ort->gatherDemandIndEndUse(12, jEndUse);                 // propane
                    collapsedIndEndUse(9, jEndUse) = ort->gatherDemandIndEndUse(13, jEndUse);                 // otherfuel1
                    collapsedIndEndUse(10, jEndUse) = ort->gatherDemandIndEndUse(14, jEndUse);                // otherfuel2
                    collapsedIndEndUse(11, jEndUse) = ort->gatherDemandIndEndUse(3, jEndUse);                 // purchased cooling
                    collapsedIndEndUse(12, jEndUse) = ort->gatherDemandIndEndUse(distrHeatSelected, jEndUse); // district heating
                    collapsedIndEndUse(13, jEndUse) = ort->gatherDemandIndEndUse(7, jEndUse);                 // water
                }
                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                        collapsedIndEndUseSub(kEndUseSub, jEndUse, 1) = ort->gatherDemandIndEndUseSub(kEndUseSub, jEndUse, 1);   // electricity
                        collapsedIndEndUseSub(kEndUseSub, jEndUse, 2) = ort->gatherDemandIndEndUseSub(kEndUseSub, jEndUse, 2);   // natural gas
                        collapsedIndEndUseSub(kEndUseSub, jEndUse, 3) = ort->gatherDemandIndEndUseSub(kEndUseSub, jEndUse, 6);   // gasoline
                        collapsedIndEndUseSub(kEndUseSub, jEndUse, 4) = ort->gatherDemandIndEndUseSub(kEndUseSub, jEndUse, 8);   // diesel
                        collapsedIndEndUseSub(kEndUseSub, jEndUse, 5) = ort->gatherDemandIndEndUseSub(kEndUseSub, jEndUse, 9);   // coal
                        collapsedIndEndUseSub(kEndUseSub, jEndUse, 6) = ort->gatherDemandIndEndUseSub(kEndUseSub, jEndUse, 10);  // fuel oil no 1
                        collapsedIndEndUseSub(kEndUseSub, jEndUse, 7) = ort->gatherDemandIndEndUseSub(kEndUseSub, jEndUse, 11);  // fuel oil no 2
                        collapsedIndEndUseSub(kEndUseSub, jEndUse, 8) = ort->gatherDemandIndEndUseSub(kEndUseSub, jEndUse, 12);  // propane
                        collapsedIndEndUseSub(kEndUseSub, jEndUse, 9) = ort->gatherDemandIndEndUseSub(kEndUseSub, jEndUse, 13);  // otherfuel1
                        collapsedIndEndUseSub(kEndUseSub, jEndUse, 10) = ort->gatherDemandIndEndUseSub(kEndUseSub, jEndUse, 14); // otherfuel2
                        collapsedIndEndUseSub(kEndUseSub, jEndUse, 11) = ort->gatherDemandIndEndUseSub(kEndUseSub, jEndUse, 3);  // purch cooling
                        collapsedIndEndUseSub(kEndUseSub, jEndUse, 12) =
                            ort->gatherDemandIndEndUseSub(kEndUseSub, jEndUse, distrHeatSelected);                              // district heating
                        collapsedIndEndUseSub(kEndUseSub, jEndUse, 13) = ort->gatherDemandIndEndUseSub(kEndUseSub, jEndUse, 7); // water
                    }
                }

                // convert totals
                collapsedTotal(1) *= powerConversion;  // electricity
                collapsedTotal(2) *= powerConversion;  // natural gas
                collapsedTotal(3) *= powerConversion;  // gasoline
                collapsedTotal(4) *= powerConversion;  // diesel
                collapsedTotal(5) *= powerConversion;  // coal
                collapsedTotal(6) *= powerConversion;  // fuel oil no 1
                collapsedTotal(7) *= powerConversion;  // fuel oil no 2
                collapsedTotal(8) *= powerConversion;  // propane
                collapsedTotal(9) *= powerConversion;  // otherfuel1
                collapsedTotal(10) *= powerConversion; // otherfuel2
                collapsedTotal(11) *= powerConversion; // purchased cooling
                collapsedTotal(12) *= powerConversion; // district heating
                collapsedTotal(13) *= flowConversion;  // water
                //---- End Use Sub-Table
                rowHead.allocate(17);
                columnHead.allocate(13);
                columnWidth.allocate(13);
                columnWidth = 10; // array assignment - same for all columns
                tableBody.allocate(13, 17);
                for (iResource = 1; iResource <= 13; ++iResource) {
                    useVal(iResource, 1) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Heating));
                    useVal(iResource, 2) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Cooling));
                    useVal(iResource, 3) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::InteriorLights));
                    useVal(iResource, 4) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::ExteriorLights));
                    useVal(iResource, 5) =
                        collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::InteriorEquipment));
                    useVal(iResource, 6) =
                        collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::ExteriorEquipment));
                    useVal(iResource, 7) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Fans));
                    useVal(iResource, 8) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Pumps));
                    useVal(iResource, 9) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::HeatRejection));
                    useVal(iResource, 10) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Humidification));
                    useVal(iResource, 11) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::HeatRecovery));
                    useVal(iResource, 12) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::WaterSystem));
                    useVal(iResource, 13) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Refrigeration));
                    useVal(iResource, 14) = collapsedEndUse(iResource, DataGlobalConstants::iEndUse.at(DataGlobalConstants::EndUse::Cogeneration));
                    useVal(iResource, 15) = collapsedTotal(iResource); // totals
                }

                rowHead(1) = "Time of Peak";
                rowHead(2) = "Heating";
                rowHead(3) = "Cooling";
                rowHead(4) = "Interior Lighting";
                rowHead(5) = "Exterior Lighting";
                rowHead(6) = "Interior Equipment";
                rowHead(7) = "Exterior Equipment";
                rowHead(8) = "Fans";
                rowHead(9) = "Pumps";
                rowHead(10) = "Heat Rejection";
                rowHead(11) = "Humidification";
                rowHead(12) = "Heat Recovery";
                rowHead(13) = "Water Systems";
                rowHead(14) = "Refrigeration";
                rowHead(15) = "Generators";
                rowHead(16) = "";
                rowHead(17) = "Total End Uses";

                if (unitsStyle_temp == iUnitsStyle::InchPound) {
                    columnHead(1) = "Electricity [kBtuh]";
                    columnHead(2) = "Natural Gas [kBtuh]";
                    columnHead(3) = "Gasoline [kBtuh]";
                    columnHead(4) = "Diesel [kBtuh]";
                    columnHead(5) = "Coal [kBtuh]";
                    columnHead(6) = "Fuel Oil No 1 [kBtuh]";
                    columnHead(7) = "Fuel Oil No 2 [kBtuh]";
                    columnHead(8) = "Propane [kBtuh]";
                    columnHead(9) = "Other Fuel 1 [kBtuh]";
                    columnHead(10) = "Other Fuel 2 [kBtuh]";
                    columnHead(11) = "District Cooling [kBtuh]";
                    {
                        auto const SELECT_CASE_var(distrHeatSelected);
                        if (SELECT_CASE_var == 4) {
                            columnHead(12) = "District Heating [kBtuh]";
                        } else if (SELECT_CASE_var == 5) {
                            columnHead(12) = "Steam [kBtuh]";
                        }
                    }
                    columnHead(13) = "Water [gal/min]";
                } else {
                    columnHead(1) = "Electricity [W]";
                    columnHead(2) = "Natural Gas [W]";
                    columnHead(3) = "Gasoline [W]";
                    columnHead(4) = "Diesel [W]";
                    columnHead(5) = "Coal [W]";
                    columnHead(6) = "Fuel Oil No 1 [W]";
                    columnHead(7) = "Fuel Oil No 2 [W]";
                    columnHead(8) = "Propane [W]";
                    columnHead(9) = "Other Fuel 1 [W]";
                    columnHead(10) = "Other Fuel 2 [W]";
                    columnHead(11) = "District Cooling [W]";
                    {
                        auto const SELECT_CASE_var(distrHeatSelected);
                        if (SELECT_CASE_var == 4) {
                            columnHead(12) = "District Heating [W]";
                        } else if (SELECT_CASE_var == 5) {
                            columnHead(12) = "Steam [W]";
                        }
                    }
                    columnHead(13) = "Water [m3/s]";
                }

                tableBody = "";
                for (iResource = 1; iResource <= 13; ++iResource) {
                    for (size_t jEndUse = 1; jEndUse <= 14; ++jEndUse) {
                        tableBody(iResource, 1 + jEndUse) = RealToStr(useVal(iResource, jEndUse), 2);
                    }
                    tableBody(iResource, 1) = DateToString(collapsedTimeStep(iResource));
                    tableBody(iResource, 17) = RealToStr(collapsedTotal(iResource), 2);
                }

                // complete the LEED end use table using the same values
                unconvert = 1 / powerConversion;

                if (produceTabular == true) {
                    WriteSubtitle(state, "End Uses");
                    WriteTable(state, tableBody, rowHead, columnHead, columnWidth, false, footnote);
                }
                if (produceSQLite == true) {
                    if (sqlite) {
                        sqlite->createSQLiteTabularDataRecords(
                            tableBody, rowHead, columnHead, "DemandEndUseComponentsSummary", "Entire Facility", "End Uses");
                    }
                }
                if (produceTabular == true) {
                    if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                        ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                            tableBody, rowHead, columnHead, "Demand End Use Components Summary", "Entire Facility", "End Uses");
                    }
                }

                //---- End Uses By Subcategory Sub-Table
                numRows = 0;
                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    if (state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories > 0) {
                        for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                            ++numRows;
                        }
                    } else {
                        ++numRows;
                    }
                }

                rowHead.allocate(numRows);
                columnHead.allocate(14);
                columnWidth.allocate(14);
                columnWidth = 10; // array assignment - same for all columns
                tableBody.allocate(14, numRows);

                rowHead = "";
                tableBody = "";

                // Build row head and subcategories columns
                i = 1;
                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    rowHead(i) = state.dataOutputProcessor->EndUseCategory(jEndUse).DisplayName;
                    if (state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories > 0) {
                        for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                            tableBody(1, i) = state.dataOutputProcessor->EndUseCategory(jEndUse).SubcategoryName(kEndUseSub);
                            ++i;
                        }
                    } else {
                        tableBody(1, i) = "General";
                        ++i;
                    }
                }

                if (unitsStyle_temp == iUnitsStyle::InchPound) {
                    columnHead(1) = "Subcategory";
                    columnHead(2) = "Electricity [kBtuh]";
                    columnHead(3) = "Natural Gas [kBtuh]";
                    columnHead(4) = "Gasoline [kBtuh]";
                    columnHead(5) = "Diesel [kBtuh]";
                    columnHead(6) = "Coal [kBtuh]";
                    columnHead(7) = "Fuel Oil No 1 [kBtuh]";
                    columnHead(8) = "Fuel Oil No 2 [kBtuh]";
                    columnHead(9) = "Propane [kBtuh]";
                    columnHead(10) = "Other Fuel 1 [kBtuh]";
                    columnHead(11) = "Other Fuel 2 [kBtuh]";
                    columnHead(12) = "District Cooling [kBtuh]";
                    {
                        auto const SELECT_CASE_var(distrHeatSelected);
                        if (SELECT_CASE_var == 4) {
                            columnHead(13) = "District Heating [kBtuh]";
                        } else if (SELECT_CASE_var == 5) {
                            columnHead(13) = "Steam [kBtuh]";
                        }
                    }
                    columnHead(14) = "Water [gal/min]";
                } else {
                    columnHead(1) = "Subcategory";
                    columnHead(2) = "Electricity [W]";
                    columnHead(3) = "Natural Gas [W]";
                    columnHead(4) = "Gasoline [W]";
                    columnHead(5) = "Diesel [W]";
                    columnHead(6) = "Coal [W]";
                    columnHead(7) = "Fuel Oil No 1 [W]";
                    columnHead(8) = "Fuel Oil No 2 [W]";
                    columnHead(9) = "Propane [W]";
                    columnHead(10) = "Other Fuel 1 [W]";
                    columnHead(11) = "Other Fuel 2 [W]";
                    columnHead(12) = "District Cooling [W]";
                    {
                        auto const SELECT_CASE_var(distrHeatSelected);
                        if (SELECT_CASE_var == 4) {
                            columnHead(13) = "District Heating [W]";
                        } else if (SELECT_CASE_var == 5) {
                            columnHead(13) = "Steam [W]";
                        }
                    }
                    columnHead(14) = "Water [m3/s]";
                }

                for (iResource = 1; iResource <= 13; ++iResource) {
                    i = 1;
                    for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                        if (state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories > 0) {
                            for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                                tableBody(iResource + 1, i) = RealToStr(collapsedEndUseSub(kEndUseSub, jEndUse, iResource), 2);
                                ++i;
                            }
                        } else {
                            tableBody(iResource + 1, i) = RealToStr(collapsedEndUse(iResource, jEndUse), 2);
                            ++i;
                        }
                    }
                }

                // heading for the entire sub-table
                if (produceTabular == true) {
                    WriteSubtitle(state, "End Uses By Subcategory");
                    WriteTable(state, tableBody, rowHead, columnHead, columnWidth, false, footnote);
                }

                Array1D_string rowHeadTemp(rowHead);
                // Before outputing to SQL, we forward fill the End use column (rowHead) (cf #7481)
                // for better sql queries
                FillRowHead(rowHeadTemp);

                for (int i = 1; i <= numRows; ++i) {
                    rowHeadTemp(i) = rowHeadTemp(i) + ":" + tableBody(1, i);
                }

                // Erase the SubCategory (first column), using slicing
                Array2D_string tableBodyTemp(tableBody({2, _, _}, {_, _, _}));
                Array1D_string columnHeadTemp(columnHead({2, _, _}));

                if (produceSQLite == true) {
                    if (sqlite) {
                        sqlite->createSQLiteTabularDataRecords(tableBodyTemp,
                                                               rowHeadTemp,
                                                               columnHeadTemp,
                                                               "DemandEndUseComponentsSummary",
                                                               "Entire Facility",
                                                               "End Uses By Subcategory");
                    }
                }

                if (produceTabular == true) {
                    if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                        ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(tableBodyTemp,
                                                                                                    rowHeadTemp,
                                                                                                    columnHeadTemp,
                                                                                                    "Demand End Use Components Summary",
                                                                                                    "Entire Facility",
                                                                                                    "End Uses By Subcategory");
                    }
                }
                rowHeadTemp.deallocate();
                tableBodyTemp.deallocate();
                columnHeadTemp.deallocate();
            }

            // EAp2-4/5. Performance Rating Method Compliance
            for (iResource = 1; iResource <= 13; ++iResource) {
                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    if (ort->needOtherRowLEED45(jEndUse)) {
                        if (state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories == 0) {
                            endUseSubOther(iResource, jEndUse) =
                                collapsedIndEndUse(iResource, jEndUse); // often the case that no subcategories are defined
                        } else {
                            Real64 sumOfSubcategories = 0.;
                            for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                                sumOfSubcategories += collapsedIndEndUseSub(kEndUseSub, jEndUse, iResource);
                            }
                            endUseSubOther(iResource, jEndUse) = collapsedIndEndUse(iResource, jEndUse) - sumOfSubcategories;
                            if (endUseSubOther(iResource, jEndUse) < 0.) {
                                endUseSubOther(iResource, jEndUse) = 0.;
                            }
                        }
                    }
                }
            }

            Array1D_int resource_entry_map;
            resource_entry_map.allocate(12);
            resource_entry_map(1) = state.dataOutRptPredefined->pdchLeedPerfElDem;          // electricity
            resource_entry_map(2) = state.dataOutRptPredefined->pdchLeedPerfGasDem;         // natural gas
            resource_entry_map(3) = state.dataOutRptPredefined->pdchLeedPerfGasolineDem;    // gasoline
            resource_entry_map(4) = state.dataOutRptPredefined->pdchLeedPerfDieselDem;      // diesel
            resource_entry_map(5) = state.dataOutRptPredefined->pdchLeedPerfCoalDem;        // coal
            resource_entry_map(6) = state.dataOutRptPredefined->pdchLeedPerfFuelOil1Dem;    // fuel oil no 1
            resource_entry_map(7) = state.dataOutRptPredefined->pdchLeedPerfFuelOil2Dem;    // fuel oil no 2
            resource_entry_map(8) = state.dataOutRptPredefined->pdchLeedPerfPropaneDem;     // propane
            resource_entry_map(9) = state.dataOutRptPredefined->pdchLeedPerfOtherFuel1Dem;  // other fuel 1
            resource_entry_map(10) = state.dataOutRptPredefined->pdchLeedPerfOtherFuel2Dem; // other fuel 2
            resource_entry_map(11) = state.dataOutRptPredefined->pdchLeedPerfDisClDem;      // district cooling
            resource_entry_map(12) = state.dataOutRptPredefined->pdchLeedPerfDisHtDem;      // district heating

            for (iResource = 1; iResource <= 12; ++iResource) {
                i = 1;
                for (size_t jEndUse = 1; jEndUse <= DataGlobalConstants::iEndUse.size(); ++jEndUse) {
                    if (state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories > 0) {
                        for (kEndUseSub = 1; kEndUseSub <= state.dataOutputProcessor->EndUseCategory(jEndUse).NumSubcategories; ++kEndUseSub) {
                            PreDefTableEntry(state,
                                             resource_entry_map(iResource),
                                             state.dataOutputProcessor->EndUseCategory(jEndUse).DisplayName + " -- " +
                                                 state.dataOutputProcessor->EndUseCategory(jEndUse).SubcategoryName(kEndUseSub),
                                             collapsedIndEndUseSub(kEndUseSub, jEndUse, iResource));
                            ++i;
                        }
                        // put other
                        if (ort->needOtherRowLEED45(jEndUse)) {
                            PreDefTableEntry(state,
                                             resource_entry_map(iResource),
                                             state.dataOutputProcessor->EndUseCategory(jEndUse).DisplayName + " -- Other",
                                             endUseSubOther(iResource, jEndUse));
                            ++i;
                        }
                    } else {
                        PreDefTableEntry(state,
                                         resource_entry_map(iResource),
                                         state.dataOutputProcessor->EndUseCategory(jEndUse).DisplayName + " -- Not Subdivided",
                                         collapsedIndEndUse(iResource, jEndUse));
                        ++i;
                    }
                }
            }
        }
    }

     void WriteCompCostTable(EnergyPlusData &state)
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         BGriffith
        //       DATE WRITTEN   April/May 2004
        //       MODIFIED       January 2010, Kyle Benne
        //                      Added SQLite output
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // produce a results table from Cost Estimate Calculations

        // METHODOLOGY EMPLOYED:
        // USE data from CostEstimateManager, call JGlazer's subroutines

        // REFERENCES:
        // na

        // Using/Aliasing

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        Array2D<Real64> TableBodyData(3, 10);
        Real64 RefBldgConstCost;   // holds interim value for construction component costs: reference bldg.
        Real64 CurntBldgConstCost; // holds interim value for construction component costs: current bldg.
        Array1D_string columnHead;
        Array1D_int columnWidth;
        Array1D_string rowHead;
        Array2D_string tableBody;
        int item;    // do-loop counter for line items
        int NumRows; // number of rows in report table excluding table header
        int NumCols; // number of columns in report table
        static std::string SIunit;
        static std::string m2_unitName;
        static Real64 m2_unitConv(0.0);
        static int unitConvIndex(0);
        static std::string IPunitName;
        Real64 IPqty;
        Real64 IPsingleValue;
        Real64 IPvaluePer;

        auto &ort(state.dataOutRptTab);

        if (!state.dataCostEstimateManager->DoCostEstimate) return;

        WriteReportHeaders(state, "Component Cost Economics Summary", "Entire Facility", OutputProcessor::StoreType::Averaged);

        // compute floor area if no ABUPS
        if (ort->buildingConditionedFloorArea == 0.0) {
            DetermineBuildingFloorArea(state);
        }

        // 1st sub-table with total Costs and normalized with area
        rowHead.allocate(10);
        columnHead.allocate(3);
        columnWidth.allocate(3);
        columnWidth = 14; // array assignment - same for all columns
        tableBody.allocate(3, 10);

        columnHead(1) = "Reference Bldg.";
        columnHead(2) = "Current Bldg. Model";
        columnHead(3) = "Difference";

        rowHead(1) = "Line Item SubTotal (~~$~~)";
        rowHead(2) = "Misc. Costs (~~$~~)";
        rowHead(3) = "Regional Adjustment (~~$~~)";
        rowHead(4) = "Design Fee (~~$~~)";
        rowHead(5) = "Contractor Fee (~~$~~)";
        rowHead(6) = "Contingency (~~$~~)";
        rowHead(7) = "Permits, Bonds, Insurance (~~$~~)";
        rowHead(8) = "Commissioning (~~$~~)";
        rowHead(9) = "Cost Estimate Total (~~$~~)";
        if (ort->unitsStyle == iUnitsStyle::InchPound) {
            SIunit = "[m2]";
            LookupSItoIP(state, SIunit, unitConvIndex, m2_unitName);
            m2_unitConv = ConvertIP(state, unitConvIndex, 1.0);
            rowHead(10) = "Cost Per Conditioned Building Area (~~$~~/ft2)";
        } else {
            rowHead(10) = "Cost Per Conditioned Building Area (~~$~~/m2)";
            m2_unitConv = 1.0;
        }
        TableBodyData = 0.0;
        tableBody = "";

        TableBodyData(1, 1) = state.dataCostEstimateManager->RefrncBldg.LineItemTot;
        tableBody(1, 1) = RealToStr(TableBodyData(1, 1), 2);
        TableBodyData(1, 2) = state.dataCostEstimateManager->RefrncBldg.MiscCostperSqMeter * ort->buildingConditionedFloorArea;
        tableBody(1, 2) = RealToStr(TableBodyData(1, 2), 2);

        if (state.dataCostEstimateManager->RefrncBldg.RegionalModifier != 1.0) {
            TableBodyData(1, 3) =
                (state.dataCostEstimateManager->RefrncBldg.LineItemTot + state.dataCostEstimateManager->RefrncBldg.MiscCostperSqMeter * ort->buildingConditionedFloorArea) * (state.dataCostEstimateManager->RefrncBldg.RegionalModifier - 1.0);
        } else {
            TableBodyData(1, 3) = 0.0;
        }

        RefBldgConstCost = sum(TableBodyData(1, {1, 3}));

        tableBody(1, 3) = RealToStr(TableBodyData(1, 3), 2);
        TableBodyData(1, 4) = RefBldgConstCost * state.dataCostEstimateManager->RefrncBldg.DesignFeeFrac;
        tableBody(1, 4) = RealToStr(TableBodyData(1, 4), 2);
        TableBodyData(1, 5) = RefBldgConstCost * state.dataCostEstimateManager->RefrncBldg.ContractorFeeFrac;
        tableBody(1, 5) = RealToStr(TableBodyData(1, 5), 2);
        TableBodyData(1, 6) = RefBldgConstCost * state.dataCostEstimateManager->RefrncBldg.ContingencyFrac;
        tableBody(1, 6) = RealToStr(TableBodyData(1, 6), 2);
        TableBodyData(1, 7) = RefBldgConstCost * state.dataCostEstimateManager->RefrncBldg.BondCostFrac;
        tableBody(1, 7) = RealToStr(TableBodyData(1, 7), 2);
        TableBodyData(1, 8) = RefBldgConstCost * state.dataCostEstimateManager->RefrncBldg.CommissioningFrac;
        tableBody(1, 8) = RealToStr(TableBodyData(1, 8), 2);
        state.dataCostEstimateManager->RefrncBldg.GrandTotal = sum(TableBodyData(1, {1, 8}));
        TableBodyData(1, 9) = state.dataCostEstimateManager->RefrncBldg.GrandTotal;
        tableBody(1, 9) = RealToStr(TableBodyData(1, 9), 2);
        if (ort->buildingConditionedFloorArea > 0.0) {
            TableBodyData(1, 10) = TableBodyData(1, 9) / (ort->buildingConditionedFloorArea * m2_unitConv);
        }
        tableBody(1, 10) = RealToStr(TableBodyData(1, 10), 2);

        TableBodyData(2, 1) = state.dataCostEstimateManager->CurntBldg.LineItemTot;
        tableBody(2, 1) = RealToStr(TableBodyData(2, 1), 2);
        TableBodyData(2, 2) = state.dataCostEstimateManager->CurntBldg.MiscCostperSqMeter * ort->buildingConditionedFloorArea;
        tableBody(2, 2) = RealToStr(TableBodyData(2, 2), 2);
        if (state.dataCostEstimateManager->CurntBldg.RegionalModifier != 1.0) {
            TableBodyData(2, 3) =
                (state.dataCostEstimateManager->CurntBldg.LineItemTot + state.dataCostEstimateManager->CurntBldg.MiscCostperSqMeter * ort->buildingConditionedFloorArea) * (state.dataCostEstimateManager->CurntBldg.RegionalModifier - 1.0);
        } else {
            TableBodyData(2, 3) = 0.0;
        }
        tableBody(2, 3) = RealToStr(TableBodyData(2, 3), 2);

        CurntBldgConstCost = sum(TableBodyData(2, {1, 3}));

        TableBodyData(2, 4) = CurntBldgConstCost * state.dataCostEstimateManager->CurntBldg.DesignFeeFrac;
        tableBody(2, 4) = RealToStr(TableBodyData(2, 4), 2);

        TableBodyData(2, 5) = CurntBldgConstCost * state.dataCostEstimateManager->CurntBldg.ContractorFeeFrac;
        tableBody(2, 5) = RealToStr(TableBodyData(2, 5), 2);
        TableBodyData(2, 6) = CurntBldgConstCost * state.dataCostEstimateManager->CurntBldg.ContingencyFrac;
        tableBody(2, 6) = RealToStr(TableBodyData(2, 6), 2);
        TableBodyData(2, 7) = CurntBldgConstCost * state.dataCostEstimateManager->CurntBldg.BondCostFrac;
        tableBody(2, 7) = RealToStr(TableBodyData(2, 7), 2);
        TableBodyData(2, 8) = CurntBldgConstCost * state.dataCostEstimateManager->CurntBldg.CommissioningFrac;
        tableBody(2, 8) = RealToStr(TableBodyData(2, 8), 2);

        state.dataCostEstimateManager->CurntBldg.GrandTotal = sum(TableBodyData(2, {1, 8}));
        TableBodyData(2, 9) = state.dataCostEstimateManager->CurntBldg.GrandTotal;
        tableBody(2, 9) = RealToStr(TableBodyData(2, 9), 2);
        if (ort->buildingConditionedFloorArea > 0) {
            TableBodyData(2, 10) = TableBodyData(2, 9) / (ort->buildingConditionedFloorArea * m2_unitConv);
        }
        tableBody(2, 10) = RealToStr(TableBodyData(2, 10), 2);

        TableBodyData(3, {1, 10}) = TableBodyData(2, {1, 10}) - TableBodyData(1, {1, 10});
        tableBody(3, 1) = RealToStr(TableBodyData(3, 1), 2);
        tableBody(3, 2) = RealToStr(TableBodyData(3, 2), 2);
        tableBody(3, 3) = RealToStr(TableBodyData(3, 3), 2);
        tableBody(3, 4) = RealToStr(TableBodyData(3, 4), 2);
        tableBody(3, 5) = RealToStr(TableBodyData(3, 5), 2);
        tableBody(3, 6) = RealToStr(TableBodyData(3, 6), 2);
        tableBody(3, 7) = RealToStr(TableBodyData(3, 7), 2);
        tableBody(3, 8) = RealToStr(TableBodyData(3, 8), 2);
        tableBody(3, 9) = RealToStr(TableBodyData(3, 9), 2);
        tableBody(3, 10) = RealToStr(TableBodyData(3, 10), 2);

        WriteSubtitle(state, "Construction Cost Estimate Summary");
        WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
        if (sqlite) {
            sqlite->createSQLiteTabularDataRecords(
                tableBody, rowHead, columnHead, "Construction Cost Estimate Summary", "Entire Facility", "Construction Cost Estimate Summary");
        }
        if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
            ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                tableBody, rowHead, columnHead, "Construction Cost Estimate Summary", "Entire Facility", "Construction Cost Estimate Summary");
        }

        NumRows = state.dataCostEstimateManager->NumLineItems + 1; // body will have the total and line items
        NumCols = 6;                // Line no., Line name, Qty, Units, ValperQty, Subtotal
        rowHead.allocate(NumRows);
        columnHead.allocate(NumCols);
        columnWidth.dimension(NumCols, 14); // array assignment - same for all columns
        tableBody.allocate(NumCols, NumRows);
        tableBody = "--";                        // array init
        rowHead = "--";                          // array init
        rowHead(NumRows) = "Line Item SubTotal"; // last line in table will be a total
        // setup up column headers
        columnHead(1) = "Line No.";
        columnHead(2) = "Item Name";
        columnHead(3) = "Quantity.";
        columnHead(4) = "Units";
        columnHead(5) = "~~$~~ per Qty.";
        columnHead(6) = "SubTotal ~~$~~";

        columnWidth = {7, 30, 16, 10, 16, 16}; // array assignment - for all columns

        for (item = 1; item <= state.dataCostEstimateManager->NumLineItems; ++item) {
            tableBody(1, item) = fmt::to_string(state.dataCostEstimateManager->CostLineItem(item).LineNumber);
            tableBody(2, item) = state.dataCostEstimateManager->CostLineItem(item).LineName;
            if (ort->unitsStyle == iUnitsStyle::InchPound) {
                LookupSItoIP(state, state.dataCostEstimateManager->CostLineItem(item).Units, unitConvIndex, IPunitName);
                if (unitConvIndex != 0) {
                    IPqty = ConvertIP(state, unitConvIndex, state.dataCostEstimateManager->CostLineItem(item).Qty);
                    tableBody(3, item) = RealToStr(IPqty, 2);
                    tableBody(4, item) = IPunitName;
                    IPsingleValue = ConvertIP(state, unitConvIndex, 1.0);
                    if (IPsingleValue != 0.0) {
                        IPvaluePer = state.dataCostEstimateManager->CostLineItem(item).ValuePer / IPsingleValue;
                        tableBody(5, item) = RealToStr(IPvaluePer, 2);
                    }
                } else {
                    tableBody(3, item) = RealToStr(state.dataCostEstimateManager->CostLineItem(item).Qty, 2);
                    tableBody(4, item) = state.dataCostEstimateManager->CostLineItem(item).Units;
                    tableBody(5, item) = RealToStr(state.dataCostEstimateManager->CostLineItem(item).ValuePer, 2);
                }
            } else {
                tableBody(3, item) = RealToStr(state.dataCostEstimateManager->CostLineItem(item).Qty, 2);
                tableBody(4, item) = state.dataCostEstimateManager->CostLineItem(item).Units;
                tableBody(5, item) = RealToStr(state.dataCostEstimateManager->CostLineItem(item).ValuePer, 2);
            }
            tableBody(6, item) = RealToStr(state.dataCostEstimateManager->CostLineItem(item).LineSubTotal, 2);
        }
        tableBody(6, NumRows) = RealToStr(state.dataCostEstimateManager->CurntBldg.LineItemTot, 2);
        WriteSubtitle(state, "Cost Line Item Details"); //: '//TRIM(RealToStr(CostEstimateTotal, 2)))
        WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
        if (sqlite) {
            sqlite->createSQLiteTabularDataRecords(
                tableBody, rowHead, columnHead, "Construction Cost Estimate Summary", "Entire Facility", "Cost Line Item Details");
        }
        if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
            ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                tableBody, rowHead, columnHead, "Construction Cost Estimate Summary", "Entire Facility", "Cost Line Item Details");
        }
    }

    void WriteVeriSumTable(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   June 2006
        //       MODIFIED       Jan. 2010, Kyle Benne. Added SQLite output
        //                      Aug. 2015, Sang Hoon Lee. Added a new table for hybrid modeling multiplier output.
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Summarize inputs and results for use with code and beyond-code
        //   compliance into a tabular report for output.

        // METHODOLOGY EMPLOYED:
        //   Create arrays for the call to WriteTable and then call it.
        //   This report actually consists of many sub-tables each with
        //   its own call to WriteTable.

        // REFERENCES:
        // na

        // Using/Aliasing
        using DataHeatBalance::BuildingAzimuth;
        using DataHeatBalance::BuildingRotationAppendixG;
        using DataHeatBalance::Lights;
        using DataHeatBalance::People;
        using DataHeatBalance::TotElecEquip;
        using DataHeatBalance::TotGasEquip;
        using DataHeatBalance::TotHWEquip;
        using DataHeatBalance::TotLights;
        using DataHeatBalance::TotOthEquip;
        using DataHeatBalance::TotPeople;
        using DataHeatBalance::Zone;
        using DataHeatBalance::ZoneData;
        using DataHeatBalance::ZoneElectric;
        using DataHeatBalance::ZoneGas;
        using DataHeatBalance::ZoneHWEq;
        using DataHeatBalance::ZoneOtherEq;
        using DataStringGlobals::VerString;
        using DataSurfaces::ExternalEnvironment;
        using DataSurfaces::FrameDivider;
        using DataSurfaces::Ground;
        using DataSurfaces::GroundFCfactorMethod;
        using DataSurfaces::KivaFoundation;
        using DataSurfaces::OtherSideCondModeledExt;
        using DataSurfaces::Surface;
        using DataSurfaces::SurfaceClass;
        using DataSurfaces::TotSurfaces;

        using General::SafeDivide;
        using ScheduleManager::GetScheduleName;
        using ScheduleManager::ScheduleAverageHoursPerWeek;
        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        int const wwrcTotal(1);
        int const wwrcNorth(2);
        int const wwrcEast(3);
        int const wwrcSouth(4);
        int const wwrcWest(5);
        int const wwrrWall(1);
        int const wwrrAbvGndWall(2);
        int const wwrrWindow(3);
        int const wwrrWWR(4);
        int const wwrrAbvGndWWR(5);

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:

        // all arrays are in the format: (row, column)
        Array1D_string columnHead;
        Array1D_int columnWidth;
        Array1D_string rowHead;
        Array2D_string tableBody;

        int iSurf;
        int kOpaque;
        int zonePt;
        int iLight;
        int iZone;
        int iPeople;
        int iPlugProc;
        int NumOfCol;
        Real64 mult;
        Real64 curAzimuth;
        Real64 curArea;
        Real64 wallAreaN;
        Real64 wallAreaS;
        Real64 wallAreaE;
        Real64 wallAreaW;
        Real64 aboveGroundWallAreaN;
        Real64 aboveGroundWallAreaS;
        Real64 aboveGroundWallAreaE;
        Real64 aboveGroundWallAreaW;
        Real64 windowAreaN;
        Real64 windowAreaS;
        Real64 windowAreaE;
        Real64 windowAreaW;
        // wall and window areas attached to conditioned zones
        Real64 wallAreaNcond;
        Real64 wallAreaScond;
        Real64 wallAreaEcond;
        Real64 wallAreaWcond;
        Real64 aboveGroundWallAreaNcond;
        Real64 aboveGroundWallAreaScond;
        Real64 aboveGroundWallAreaEcond;
        Real64 aboveGroundWallAreaWcond;
        Real64 windowAreaNcond;
        Real64 windowAreaScond;
        Real64 windowAreaEcond;
        Real64 windowAreaWcond;
        bool isConditioned;
        bool isAboveGround;

        Real64 roofArea;
        Real64 skylightArea;
        Real64 totLightPower;
        Real64 totNumPeople;
        Real64 totPlugProcess;
        Real64 frameWidth;
        Real64 frameArea;

        bool zoneIsCond;
        bool usezoneFloorArea;

        static int grandTotal(1);
        static int condTotal(2);
        static int uncondTotal(3);
        static int notpartTotal(4);
        int iTotal;
        static std::string SIunit;
        static int unitConvIndex(0);
        static Real64 m_unitConv(0.0);
        static Real64 m2_unitConv(0.0);
        static Real64 m3_unitConv(0.0);
        static Real64 Wm2_unitConv(0.0);
        static std::string m_unitName;
        static std::string m2_unitName;
        static std::string m3_unitName;
        static std::string Wm2_unitName;

        // zone summary total
        static Array1D<Real64> zstArea(4);
        static Array1D<Real64> zstVolume(4);
        static Array1D<Real64> zstWallArea(4);
        static Array1D<Real64> zstUndWallArea(4);
        static Array1D<Real64> zstWindowArea(4);
        static Array1D<Real64> zstOpeningArea(4);
        static Array1D<Real64> zstLight(4);
        static Array1D<Real64> zstPeople(4);
        static Array1D<Real64> zstPlug(4);

        zstArea = 0.0;
        zstVolume = 0.0;
        zstWallArea = 0.0;
        zstUndWallArea = 0.0;
        zstWindowArea = 0.0;
        zstOpeningArea = 0.0;
        zstLight = 0.0;
        zstPeople = 0.0;
        zstPlug = 0.0;

        // misc
        Real64 pdiff;
        bool DetailedWWR;
        Real64 TotalWallArea;
        Real64 TotalWindowArea;
        Real64 TotalAboveGroundWallArea;

        Array1D<Real64> zoneOpeningArea;
        zoneOpeningArea.allocate(state.dataGlobal->NumOfZones);
        zoneOpeningArea = 0.0;

        Array1D<Real64> zoneGlassArea;
        zoneGlassArea.allocate(state.dataGlobal->NumOfZones);
        zoneGlassArea = 0.0;
        auto &ort(state.dataOutRptTab);

        // all arrays are in the format: (row, columnm)
        if (ort->displayTabularVeriSum) {
            // show the headers of the report
            WriteReportHeaders(state, "Input Verification and Results Summary", "Entire Facility", OutputProcessor::StoreType::Averaged);

            // do unit conversions if necessary
            if (ort->unitsStyle == iUnitsStyle::InchPound) {
                SIunit = "[m]";
                LookupSItoIP(state, SIunit, unitConvIndex, m_unitName);
                m_unitConv = ConvertIP(state, unitConvIndex, 1.0);
                SIunit = "[m2]";
                LookupSItoIP(state, SIunit, unitConvIndex, m2_unitName);
                m2_unitConv = ConvertIP(state, unitConvIndex, 1.0);
                SIunit = "[m3]";
                LookupSItoIP(state, SIunit, unitConvIndex, m3_unitName);
                m3_unitConv = ConvertIP(state, unitConvIndex, 1.0);
                SIunit = "[W/m2]";
                LookupSItoIP(state, SIunit, unitConvIndex, Wm2_unitName);
                Wm2_unitConv = ConvertIP(state, unitConvIndex, 1.0);
            } else {
                m_unitName = "[m]";
                m_unitConv = 1.0;
                m2_unitName = "[m2]";
                m2_unitConv = 1.0;
                m3_unitName = "[m3]";
                m3_unitConv = 1.0;
                Wm2_unitName = "[W/m2]";
                Wm2_unitConv = 1.0;
            }
            //---- General Sub-Table

            // since a variable number of design days is possible, first read them before sizing the arrays
            rowHead.allocate(10);
            columnHead.allocate(1);
            columnWidth.allocate(1);
            columnWidth = 14; // array assignment - same for all columns
            tableBody.allocate(1, 10);

            columnHead(1) = "Value";
            rowHead(1) = "Program Version and Build";
            rowHead(2) = "RunPeriod";
            rowHead(3) = "Weather File";
            rowHead(4) = "Latitude [deg]";
            rowHead(5) = "Longitude [deg]";

            rowHead(6) = "Elevation " + m_unitName;
            rowHead(7) = "Time Zone";
            rowHead(8) = "North Axis Angle [deg]";
            rowHead(9) = "Rotation for Appendix G [deg]";
            rowHead(10) = "Hours Simulated [hrs]";
            //  rowHead(9)  = 'Num Table Entries' !used for debugging

            tableBody = "";

            tableBody(1, 1) = VerString;                               // program
            tableBody(1, 2) = state.dataEnvrn->EnvironmentName;                         // runperiod name
            tableBody(1, 3) = state.dataEnvrn->WeatherFileLocationTitle;                // weather
            tableBody(1, 4) = RealToStr(state.dataEnvrn->Latitude, 2);                  // latitude
            tableBody(1, 5) = RealToStr(state.dataEnvrn->Longitude, 2);                 // longitude
            tableBody(1, 6) = RealToStr(state.dataEnvrn->Elevation * m_unitConv, 2);    // Elevation
            tableBody(1, 7) = RealToStr(state.dataEnvrn->TimeZoneNumber, 2);            // Time Zone
            tableBody(1, 8) = RealToStr(BuildingAzimuth, 2);           // north axis angle
            tableBody(1, 9) = RealToStr(BuildingRotationAppendixG, 2); // Rotation for Appendix G
            tableBody(1, 10) = RealToStr(ort->gatherElapsedTimeBEPS, 2);    // hours simulated
            //  tableBody(9,1) = TRIM(fmt::to_string(state.dataOutRptPredefined->numTableEntry)) !number of table entries for predefined tables

            WriteSubtitle(state, "General");
            WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
            if (sqlite) {
                sqlite->createSQLiteTabularDataRecords(
                    tableBody, rowHead, columnHead, "InputVerificationandResultsSummary", "Entire Facility", "General");
            }
            if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                    tableBody, rowHead, columnHead, "Input Verification and Results Summary", "Entire Facility", "General");
            }

            //---- Window Wall Ratio Sub-Table
            WriteTextLine(state, "ENVELOPE", true);

            rowHead.allocate(5);
            columnHead.allocate(5);
            columnWidth.allocate(5);
            columnWidth = 14; // array assignment - same for all columns
            tableBody.allocate(5, 5);

            columnHead(wwrcTotal) = "Total";
            columnHead(wwrcNorth) = "North (315 to 45 deg)";
            columnHead(wwrcEast) = "East (45 to 135 deg)";
            columnHead(wwrcSouth) = "South (135 to 225 deg)";
            columnHead(wwrcWest) = "West (225 to 315 deg)";

            rowHead(wwrrWall) = "Gross Wall Area " + m2_unitName;
            rowHead(wwrrAbvGndWall) = "Above Ground Wall Area " + m2_unitName;
            rowHead(wwrrWindow) = "Window Opening Area " + m2_unitName;
            rowHead(wwrrWWR) = "Gross Window-Wall Ratio [%]";
            rowHead(wwrrAbvGndWWR) = "Above Ground Window-Wall Ratio [%]";

            wallAreaN = 0.0;
            wallAreaS = 0.0;
            wallAreaE = 0.0;
            wallAreaW = 0.0;
            aboveGroundWallAreaN = 0.0;
            aboveGroundWallAreaS = 0.0;
            aboveGroundWallAreaE = 0.0;
            aboveGroundWallAreaW = 0.0;
            windowAreaN = 0.0;
            windowAreaS = 0.0;
            windowAreaE = 0.0;
            windowAreaW = 0.0;
            wallAreaNcond = 0.0;
            wallAreaScond = 0.0;
            wallAreaEcond = 0.0;
            wallAreaWcond = 0.0;
            aboveGroundWallAreaNcond = 0.0;
            aboveGroundWallAreaScond = 0.0;
            aboveGroundWallAreaEcond = 0.0;
            aboveGroundWallAreaWcond = 0.0;
            windowAreaNcond = 0.0;
            windowAreaScond = 0.0;
            windowAreaEcond = 0.0;
            windowAreaWcond = 0.0;
            roofArea = 0.0;
            skylightArea = 0.0;
            totLightPower = 0.0;
            totNumPeople = 0.0;
            totPlugProcess = 0.0;
            kOpaque = 0;

            DetailedWWR = (inputProcessor->getNumSectionsFound("DETAILEDWWR_DEBUG") > 0);

            if (DetailedWWR) {
                print(state.files.debug, "{}\n", "======90.1 Classification [>=60 & <=120] tilt = wall==================");
                print(state.files.debug, "{}\n", "SurfName,Class,Area,Tilt");
            }

            for (iSurf = 1; iSurf <= TotSurfaces; ++iSurf) {
                // only exterior surfaces including underground
                if (!Surface(iSurf).HeatTransSurf) continue;
                isAboveGround = (Surface(iSurf).ExtBoundCond == ExternalEnvironment) || (Surface(iSurf).ExtBoundCond == OtherSideCondModeledExt);
                if (isAboveGround || (Surface(iSurf).ExtBoundCond == Ground) || (Surface(iSurf).ExtBoundCond == GroundFCfactorMethod) ||
                    (Surface(iSurf).ExtBoundCond == KivaFoundation)) {
                    curAzimuth = Surface(iSurf).Azimuth;
                    // Round to two decimals, like the display in tables
                    curAzimuth = round(curAzimuth * 100.0) / 100.0;
                    curArea = Surface(iSurf).GrossArea;
                    if (Surface(iSurf).FrameDivider != 0) {
                        frameWidth = FrameDivider(Surface(iSurf).FrameDivider).FrameWidth;
                        frameArea = (Surface(iSurf).Height + 2.0 * frameWidth) * (Surface(iSurf).Width + 2.0 * frameWidth) -
                                    (Surface(iSurf).Height * Surface(iSurf).Width);
                        curArea += frameArea;
                    }
                    zonePt = Surface(iSurf).Zone;
                    isConditioned = false;
                    if (zonePt > 0) {
                        if (Zone(zonePt).SystemZoneNodeNumber > 0) {
                            isConditioned = true;
                        }
                    }
                    if ((Surface(iSurf).Tilt >= 60.0) && (Surface(iSurf).Tilt <= 120.0)) {
                        // vertical walls and windows
                        {
                            auto const SELECT_CASE_var(Surface(iSurf).Class);
                            if ((SELECT_CASE_var == SurfaceClass::Wall) || (SELECT_CASE_var == SurfaceClass::Floor) ||
                                (SELECT_CASE_var == SurfaceClass::Roof)) {
                                mult = Zone(zonePt).Multiplier * Zone(zonePt).ListMultiplier;
                                if ((curAzimuth >= 315.0) || (curAzimuth < 45.0)) {
                                    wallAreaN += curArea * mult;
                                    if (isConditioned) wallAreaNcond += curArea * mult;
                                    if (isAboveGround) {
                                        aboveGroundWallAreaN += curArea * mult;
                                        if (isConditioned) aboveGroundWallAreaNcond += curArea * mult;
                                    }
                                } else if ((curAzimuth >= 45.0) && (curAzimuth < 135.0)) {
                                    wallAreaE += curArea * mult;
                                    if (isConditioned) wallAreaEcond += curArea * mult;
                                    if (isAboveGround) {
                                        aboveGroundWallAreaE += curArea * mult;
                                        if (isConditioned) aboveGroundWallAreaEcond += curArea * mult;
                                    }
                                } else if ((curAzimuth >= 135.0) && (curAzimuth < 225.0)) {
                                    wallAreaS += curArea * mult;
                                    if (isConditioned) wallAreaScond += curArea * mult;
                                    if (isAboveGround) {
                                        aboveGroundWallAreaS += curArea * mult;
                                        if (isConditioned) aboveGroundWallAreaScond += curArea * mult;
                                    }
                                } else if ((curAzimuth >= 225.0) && (curAzimuth < 315.0)) {
                                    wallAreaW += curArea * mult;
                                    if (isConditioned) wallAreaWcond += curArea * mult;
                                    if (isAboveGround) {
                                        aboveGroundWallAreaW += curArea * mult;
                                        if (isConditioned) aboveGroundWallAreaWcond += curArea * mult;
                                    }
                                }
                                if (DetailedWWR) {
                                    print(state.files.debug, "{},Wall,{:.1R},{:.1R}\n", Surface(iSurf).Name, curArea * mult, Surface(iSurf).Tilt);
                                }
                            } else if ((SELECT_CASE_var == SurfaceClass::Window) || (SELECT_CASE_var == SurfaceClass::TDD_Dome)) {
                                mult = Zone(zonePt).Multiplier * Zone(zonePt).ListMultiplier * Surface(iSurf).Multiplier;
                                if ((curAzimuth >= 315.0) || (curAzimuth < 45.0)) {
                                    windowAreaN += curArea * mult;
                                    if (isConditioned) windowAreaNcond += curArea * mult;
                                } else if ((curAzimuth >= 45.0) && (curAzimuth < 135.0)) {
                                    windowAreaE += curArea * mult;
                                    if (isConditioned) windowAreaEcond += curArea * mult;
                                } else if ((curAzimuth >= 135.0) && (curAzimuth < 225.0)) {
                                    windowAreaS += curArea * mult;
                                    if (isConditioned) windowAreaScond += curArea * mult;
                                } else if ((curAzimuth >= 225.0) && (curAzimuth < 315.0)) {
                                    windowAreaW += curArea * mult;
                                    if (isConditioned) windowAreaWcond += curArea * mult;
                                }
                                zoneOpeningArea(zonePt) +=
                                    curArea * Surface(iSurf).Multiplier; // total window opening area for each zone (glass plus frame area)
                                zoneGlassArea(zonePt) += Surface(iSurf).GrossArea * Surface(iSurf).Multiplier;
                                if (DetailedWWR) {
                                    print(state.files.debug, "{},Window,{:.1R},{:.1R}\n", Surface(iSurf).Name, curArea * mult, Surface(iSurf).Tilt);
                                }
                            }
                        }
                    } else if (Surface(iSurf).Tilt < 60.0) { // roof and skylights
                        {
                            auto const SELECT_CASE_var(Surface(iSurf).Class);
                            if ((SELECT_CASE_var == SurfaceClass::Wall) || (SELECT_CASE_var == SurfaceClass::Floor) ||
                                (SELECT_CASE_var == SurfaceClass::Roof)) {
                                mult = Zone(zonePt).Multiplier * Zone(zonePt).ListMultiplier;
                                roofArea += curArea * mult;
                                if (DetailedWWR) {
                                    print(state.files.debug, "{},Roof,{:.1R},{:.1R}\n", Surface(iSurf).Name, curArea * mult, Surface(iSurf).Tilt);
                                }
                            } else if ((SELECT_CASE_var == SurfaceClass::Window) || (SELECT_CASE_var == SurfaceClass::TDD_Dome)) {
                                mult = Zone(zonePt).Multiplier * Zone(zonePt).ListMultiplier * Surface(iSurf).Multiplier;
                                skylightArea += curArea * mult;
                                if (DetailedWWR) {
                                    print(state.files.debug, "{},Skylight,{:.1R},{:.1R}\n", Surface(iSurf).Name, curArea * mult, Surface(iSurf).Tilt);
                                }
                            }
                        }
                    } else { // floors
                             // ignored
                    }
                }
            }

            TotalWallArea = wallAreaN + wallAreaS + wallAreaE + wallAreaW;
            TotalAboveGroundWallArea = aboveGroundWallAreaN + aboveGroundWallAreaS + aboveGroundWallAreaE + aboveGroundWallAreaW;
            TotalWindowArea = windowAreaN + windowAreaS + windowAreaE + windowAreaW;
            if (DetailedWWR) {
                print(state.files.debug, "{}\n", "========================");
                print(state.files.debug, "{}\n", "TotalWallArea,WallAreaN,WallAreaS,WallAreaE,WallAreaW");
                print(state.files.debug, "{}\n", "TotalWindowArea,WindowAreaN,WindowAreaS,WindowAreaE,WindowAreaW");
                print(state.files.debug, "{:.2R},{:.2R},{:.2R},{:.2R},{:.2R}\n", TotalWallArea, wallAreaN, wallAreaS, wallAreaE, wallAreaW);
                print(state.files.debug, "{:.2R},{:.2R},{:.2R},{:.2R},{:.2R}\n", TotalWindowArea, windowAreaN, windowAreaS, windowAreaE, windowAreaW);
            }

            tableBody = "";

            tableBody(wwrcNorth, wwrrWall) = RealToStr(wallAreaN * m2_unitConv, 2);
            tableBody(wwrcSouth, wwrrWall) = RealToStr(wallAreaS * m2_unitConv, 2);
            tableBody(wwrcEast, wwrrWall) = RealToStr(wallAreaE * m2_unitConv, 2);
            tableBody(wwrcWest, wwrrWall) = RealToStr(wallAreaW * m2_unitConv, 2);
            tableBody(wwrcTotal, wwrrWall) = RealToStr(TotalWallArea * m2_unitConv, 2);

            tableBody(wwrcNorth, wwrrAbvGndWall) = RealToStr(aboveGroundWallAreaN * m2_unitConv, 2);
            tableBody(wwrcSouth, wwrrAbvGndWall) = RealToStr(aboveGroundWallAreaS * m2_unitConv, 2);
            tableBody(wwrcEast, wwrrAbvGndWall) = RealToStr(aboveGroundWallAreaE * m2_unitConv, 2);
            tableBody(wwrcWest, wwrrAbvGndWall) = RealToStr(aboveGroundWallAreaW * m2_unitConv, 2);
            tableBody(wwrcTotal, wwrrAbvGndWall) = RealToStr(TotalAboveGroundWallArea * m2_unitConv, 2);

            tableBody(wwrcNorth, wwrrWindow) = RealToStr(windowAreaN * m2_unitConv, 2);
            tableBody(wwrcSouth, wwrrWindow) = RealToStr(windowAreaS * m2_unitConv, 2);
            tableBody(wwrcEast, wwrrWindow) = RealToStr(windowAreaE * m2_unitConv, 2);
            tableBody(wwrcWest, wwrrWindow) = RealToStr(windowAreaW * m2_unitConv, 2);
            tableBody(wwrcTotal, wwrrWindow) = RealToStr(TotalWindowArea * m2_unitConv, 2);

            tableBody(wwrcNorth, wwrrWWR) = RealToStr(100.0 * SafeDivide(windowAreaN, wallAreaN), 2);
            tableBody(wwrcSouth, wwrrWWR) = RealToStr(100.0 * SafeDivide(windowAreaS, wallAreaS), 2);
            tableBody(wwrcEast, wwrrWWR) = RealToStr(100.0 * SafeDivide(windowAreaE, wallAreaE), 2);
            tableBody(wwrcWest, wwrrWWR) = RealToStr(100.0 * SafeDivide(windowAreaW, wallAreaW), 2);
            tableBody(wwrcTotal, wwrrWWR) = RealToStr(100.0 * SafeDivide(TotalWindowArea, TotalWallArea), 2);

            tableBody(wwrcNorth, wwrrAbvGndWWR) = RealToStr(100.0 * SafeDivide(windowAreaN, aboveGroundWallAreaN), 2);
            tableBody(wwrcSouth, wwrrAbvGndWWR) = RealToStr(100.0 * SafeDivide(windowAreaS, aboveGroundWallAreaS), 2);
            tableBody(wwrcEast, wwrrAbvGndWWR) = RealToStr(100.0 * SafeDivide(windowAreaE, aboveGroundWallAreaE), 2);
            tableBody(wwrcWest, wwrrAbvGndWWR) = RealToStr(100.0 * SafeDivide(windowAreaW, aboveGroundWallAreaW), 2);
            tableBody(wwrcTotal, wwrrAbvGndWWR) = RealToStr(100.0 * SafeDivide(TotalWindowArea, TotalAboveGroundWallArea), 2);

            WriteSubtitle(state, "Window-Wall Ratio");
            WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
            if (sqlite) {
                sqlite->createSQLiteTabularDataRecords(
                    tableBody, rowHead, columnHead, "InputVerificationandResultsSummary", "Entire Facility", "Window-Wall Ratio");
            }
            if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                    tableBody, rowHead, columnHead, "Input Verification and Results Summary", "Entire Facility", "Window-Wall Ratio");
            }

            //---- Conditioned Window Wall Ratio Sub-Table
            rowHead.allocate(5);
            columnHead.allocate(5);
            columnWidth.allocate(5);
            columnWidth = 14; // array assignment - same for all columns
            tableBody.allocate(5, 5);

            columnHead(wwrcTotal) = "Total";
            columnHead(wwrcNorth) = "North (315 to 45 deg)";
            columnHead(wwrcEast) = "East (45 to 135 deg)";
            columnHead(wwrcSouth) = "South (135 to 225 deg)";
            columnHead(wwrcWest) = "West (225 to 315 deg)";

            rowHead(wwrrWall) = "Gross Wall Area " + m2_unitName;
            rowHead(wwrrAbvGndWall) = "Above Ground Wall Area " + m2_unitName;
            rowHead(wwrrWindow) = "Window Opening Area " + m2_unitName;
            rowHead(wwrrWWR) = "Gross Window-Wall Ratio [%]";
            rowHead(wwrrAbvGndWWR) = "Above Ground Window-Wall Ratio [%]";

            // calculations appear in last block with normal window-wall ratio table

            TotalWallArea = wallAreaNcond + wallAreaScond + wallAreaEcond + wallAreaWcond;
            TotalAboveGroundWallArea = aboveGroundWallAreaNcond + aboveGroundWallAreaScond + aboveGroundWallAreaEcond + aboveGroundWallAreaWcond;
            TotalWindowArea = windowAreaNcond + windowAreaScond + windowAreaEcond + windowAreaWcond;

            tableBody = "";

            tableBody(wwrcNorth, wwrrWall) = RealToStr(wallAreaNcond * m2_unitConv, 2);
            tableBody(wwrcSouth, wwrrWall) = RealToStr(wallAreaScond * m2_unitConv, 2);
            tableBody(wwrcEast, wwrrWall) = RealToStr(wallAreaEcond * m2_unitConv, 2);
            tableBody(wwrcWest, wwrrWall) = RealToStr(wallAreaWcond * m2_unitConv, 2);
            tableBody(wwrcTotal, wwrrWall) = RealToStr(TotalWallArea * m2_unitConv, 2);

            tableBody(wwrcNorth, wwrrAbvGndWall) = RealToStr(aboveGroundWallAreaNcond * m2_unitConv, 2);
            tableBody(wwrcSouth, wwrrAbvGndWall) = RealToStr(aboveGroundWallAreaScond * m2_unitConv, 2);
            tableBody(wwrcEast, wwrrAbvGndWall) = RealToStr(aboveGroundWallAreaEcond * m2_unitConv, 2);
            tableBody(wwrcWest, wwrrAbvGndWall) = RealToStr(aboveGroundWallAreaWcond * m2_unitConv, 2);
            tableBody(wwrcTotal, wwrrAbvGndWall) = RealToStr(TotalAboveGroundWallArea * m2_unitConv, 2);

            tableBody(wwrcNorth, wwrrWindow) = RealToStr(windowAreaNcond * m2_unitConv, 2);
            tableBody(wwrcSouth, wwrrWindow) = RealToStr(windowAreaScond * m2_unitConv, 2);
            tableBody(wwrcEast, wwrrWindow) = RealToStr(windowAreaEcond * m2_unitConv, 2);
            tableBody(wwrcWest, wwrrWindow) = RealToStr(windowAreaWcond * m2_unitConv, 2);
            tableBody(wwrcTotal, wwrrWindow) = RealToStr(TotalWindowArea * m2_unitConv, 2);

            tableBody(wwrcNorth, wwrrWWR) = RealToStr(100.0 * SafeDivide(windowAreaNcond, wallAreaNcond), 2);
            tableBody(wwrcSouth, wwrrWWR) = RealToStr(100.0 * SafeDivide(windowAreaScond, wallAreaScond), 2);
            tableBody(wwrcEast, wwrrWWR) = RealToStr(100.0 * SafeDivide(windowAreaEcond, wallAreaEcond), 2);
            tableBody(wwrcWest, wwrrWWR) = RealToStr(100.0 * SafeDivide(windowAreaWcond, wallAreaWcond), 2);
            tableBody(wwrcTotal, wwrrWWR) = RealToStr(100.0 * SafeDivide(TotalWindowArea, TotalWallArea), 2);

            tableBody(wwrcNorth, wwrrAbvGndWWR) = RealToStr(100.0 * SafeDivide(windowAreaNcond, aboveGroundWallAreaNcond), 2);
            tableBody(wwrcSouth, wwrrAbvGndWWR) = RealToStr(100.0 * SafeDivide(windowAreaScond, aboveGroundWallAreaScond), 2);
            tableBody(wwrcEast, wwrrAbvGndWWR) = RealToStr(100.0 * SafeDivide(windowAreaEcond, aboveGroundWallAreaEcond), 2);
            tableBody(wwrcWest, wwrrAbvGndWWR) = RealToStr(100.0 * SafeDivide(windowAreaWcond, aboveGroundWallAreaWcond), 2);
            tableBody(wwrcTotal, wwrrAbvGndWWR) = RealToStr(100.0 * SafeDivide(TotalWindowArea, TotalAboveGroundWallArea), 2);

            WriteSubtitle(state, "Conditioned Window-Wall Ratio");
            WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
            if (sqlite) {
                sqlite->createSQLiteTabularDataRecords(
                    tableBody, rowHead, columnHead, "InputVerificationandResultsSummary", "Entire Facility", "Conditioned Window-Wall Ratio");
            }
            if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                    tableBody, rowHead, columnHead, "Input Verification and Results Summary", "Entire Facility", "Conditioned Window-Wall Ratio");
            }

            //---- Skylight Roof Ratio Sub-Table
            rowHead.allocate(3);
            columnHead.allocate(1);
            columnWidth.allocate(1);
            columnWidth = 14; // array assignment - same for all columns
            tableBody.allocate(1, 3);

            columnHead(1) = "Total";

            rowHead(1) = "Gross Roof Area " + m2_unitName;
            rowHead(2) = "Skylight Area " + m2_unitName;
            rowHead(3) = "Skylight-Roof Ratio [%]";

            if (DetailedWWR) {
                print(state.files.debug, "{}\n", "========================");
                print(state.files.debug, "{}\n", "TotalRoofArea,SkylightArea");
                print(state.files.debug, "{:.2R},{:.2R}\n", roofArea, skylightArea);
            }

            tableBody(1, 1) = RealToStr(roofArea * m2_unitConv, 2);
            tableBody(1, 2) = RealToStr(skylightArea * m2_unitConv, 2);
            tableBody(1, 3) = RealToStr(100.0 * SafeDivide(skylightArea, roofArea), 2);

            WriteSubtitle(state, "Skylight-Roof Ratio");
            WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
            if (sqlite) {
                sqlite->createSQLiteTabularDataRecords(
                    tableBody, rowHead, columnHead, "InputVerificationandResultsSummary", "Entire Facility", "Skylight-Roof Ratio");
            }
            if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                    tableBody, rowHead, columnHead, "Input Verification and Results Summary", "Entire Facility", "Skylight-Roof Ratio");
            }

            //---- Hybrid Model: Internal Thermal Mass Sub-Table
            if (FlagHybridModel_TM) {
                rowHead.allocate(state.dataGlobal->NumOfZones);
                NumOfCol = 2;
                columnHead.allocate(NumOfCol);
                columnWidth.allocate(NumOfCol);
                columnWidth = 14; // array assignment - same for all columns
                tableBody.allocate(NumOfCol, state.dataGlobal->NumOfZones);

                columnHead(1) = "Hybrid Modeling (Y/N)";
                columnHead(2) = "Temperature Capacitance Multiplier ";

                rowHead = "";
                tableBody = "";

                for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
                    rowHead(iZone) = Zone(iZone).Name;
                    if (HybridModelZone(iZone).InternalThermalMassCalc_T) {
                        tableBody(1, iZone) = "Yes";
                    } else {
                        tableBody(1, iZone) = "No";
                    }
                    tableBody(2, iZone) = RealToStr(Zone(iZone).ZoneVolCapMultpSensHMAverage, 2);
                }

                WriteSubtitle(state, "Hybrid Model: Internal Thermal Mass");
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(tableBody,
                                                           rowHead,
                                                           columnHead,
                                                           "InputVerificationandResultsSummary",
                                                           "Entire Facility",
                                                           "Hybrid Model: Internal Thermal Mass");
                }
            }

            Real64 const totExtGrossWallArea_Multiplied(sum(Zone, &ZoneData::ExtGrossWallArea_Multiplied));
            Real64 const totExtGrossGroundWallArea_Multiplied(sum(Zone, &ZoneData::ExtGrossGroundWallArea_Multiplied));
            if (totExtGrossWallArea_Multiplied > 0.0 || totExtGrossGroundWallArea_Multiplied > 0.0) {
                pdiff = std::abs((wallAreaN + wallAreaS + wallAreaE + wallAreaW) -
                                 (totExtGrossWallArea_Multiplied + totExtGrossGroundWallArea_Multiplied)) /
                        (totExtGrossWallArea_Multiplied + totExtGrossGroundWallArea_Multiplied);
                if (pdiff > 0.019) {
                    ShowWarningError(state,
                        "WriteVeriSumTable: InputVerificationsAndResultsSummary: Wall area based on [>=60,<=120] degrees (tilt) as walls");
                    ShowContinueError(
                        state,
                        format("differs ~{:.1R}% from user entered Wall class surfaces. Degree calculation based on ASHRAE 90.1 wall definitions.",
                               pdiff * 100.0));
                    //      CALL ShowContinueError(state, 'Calculated based on degrees=['//  &
                    //         TRIM(ADJUSTL(RealToStr((wallAreaN + wallAreaS + wallAreaE + wallAreaW),3)))//  &
                    //         '] m2, Calculated from user entered Wall class surfaces=['//  &
                    //         TRIM(ADJUSTL(RealToStr(SUM(Zone(1:state.dataGlobal->NumOfZones)%ExtGrossWallArea_Multiplied),3)))//' m2.')
                    ShowContinueError(state, "Check classes of surfaces and tilts for discrepancies.");
                    ShowContinueError(state, "Total wall area by ASHRAE 90.1 definition=" +
                                      stripped(RealToStr((wallAreaN + wallAreaS + wallAreaE + wallAreaW), 3)) + " m2.");
                    ShowContinueError(state, "Total exterior wall area from user entered classes=" + stripped(RealToStr(totExtGrossWallArea_Multiplied, 3)) +
                                      " m2.");
                    ShowContinueError(state, "Total ground contact wall area from user entered classes=" +
                                      stripped(RealToStr(totExtGrossGroundWallArea_Multiplied, 3)) + " m2.");
                }
            }
            //---- Space Summary Sub-Table
            WriteTextLine(state, "PERFORMANCE", true);

            rowHead.allocate(state.dataGlobal->NumOfZones + 4);
            NumOfCol = 12;
            columnHead.allocate(NumOfCol);
            columnWidth.allocate(NumOfCol);
            columnWidth = 14; // array assignment - same for all columns
            tableBody.allocate(NumOfCol, state.dataGlobal->NumOfZones + 4);

            columnHead(1) = "Area " + m2_unitName;
            columnHead(2) = "Conditioned (Y/N)";
            columnHead(3) = "Part of Total Floor Area (Y/N)";
            columnHead(4) = "Volume " + m3_unitName;
            columnHead(5) = "Multipliers";
            columnHead(6) = "Above Ground Gross Wall Area " + m2_unitName;
            columnHead(7) = "Underground Gross Wall Area " + m2_unitName;
            columnHead(8) = "Window Glass Area " + m2_unitName;
            columnHead(9) = "Opening Area " + m2_unitName;
            columnHead(10) = "Lighting " + Wm2_unitName;
            columnHead(11) = "People " + m2_unitName.substr(0, len(m2_unitName) - 1) + " per person" + m2_unitName[len(m2_unitName) - 1];
            columnHead(12) = "Plug and Process " + Wm2_unitName;

            rowHead = "";
            rowHead(state.dataGlobal->NumOfZones + grandTotal) = "Total";
            rowHead(state.dataGlobal->NumOfZones + condTotal) = "Conditioned Total";
            rowHead(state.dataGlobal->NumOfZones + uncondTotal) = "Unconditioned Total";
            rowHead(state.dataGlobal->NumOfZones + notpartTotal) = "Not Part of Total";

            tableBody = "";

            for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
                mult = Zone(iZone).Multiplier * Zone(iZone).ListMultiplier;
                rowHead(iZone) = Zone(iZone).Name;
                // Conditioned or not
                if (Zone(iZone).SystemZoneNodeNumber > 0) {
                    tableBody(2, iZone) = "Yes";
                    zoneIsCond = true;
                } else {
                    tableBody(2, iZone) = "No";
                    zoneIsCond = false;
                }
                // Part of Total Floor Area or not
                if (Zone(iZone).isPartOfTotalArea) {
                    tableBody(3, iZone) = "Yes";
                    usezoneFloorArea = true;
                } else {
                    tableBody(3, iZone) = "No";
                    usezoneFloorArea = false;
                }
                tableBody(1, iZone) = RealToStr(Zone(iZone).FloorArea * m2_unitConv, 2);
                tableBody(4, iZone) = RealToStr(Zone(iZone).Volume * m3_unitConv, 2);
                // no unit conversion necessary since done automatically
                PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedSutSpArea, Zone(iZone).Name, Zone(iZone).FloorArea, 2);
                if (zoneIsCond) {
                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedSutOcArea, Zone(iZone).Name, Zone(iZone).FloorArea, 2);
                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedSutUnArea, Zone(iZone).Name, "0.00");
                } else {
                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedSutOcArea, Zone(iZone).Name, "0.00");
                    PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedSutUnArea, Zone(iZone).Name, Zone(iZone).FloorArea, 2);
                }
                tableBody(5, iZone) = RealToStr(mult, 2);
                tableBody(6, iZone) = RealToStr(Zone(iZone).ExtGrossWallArea * m2_unitConv, 2);
                tableBody(7, iZone) = RealToStr(Zone(iZone).ExtGrossGroundWallArea * m2_unitConv, 2);
                tableBody(8, iZone) = RealToStr(zoneGlassArea(iZone) * m2_unitConv, 2);
                tableBody(9, iZone) = RealToStr(zoneOpeningArea(iZone) * m2_unitConv, 2);
                // lighting density
                totLightPower = 0.0;
                for (iLight = 1; iLight <= TotLights; ++iLight) {
                    if (iZone == Lights(iLight).ZonePtr) {
                        totLightPower += Lights(iLight).DesignLevel;
                    }
                }
                if (Zone(iZone).FloorArea > 0) {
                    tableBody(10, iZone) = RealToStr(Wm2_unitConv * totLightPower / Zone(iZone).FloorArea, 4);
                }
                // people density
                totNumPeople = 0.0;
                for (iPeople = 1; iPeople <= TotPeople; ++iPeople) {
                    if (iZone == People(iPeople).ZonePtr) {
                        totNumPeople += People(iPeople).NumberOfPeople;
                    }
                }
                if (totNumPeople > 0) {
                    tableBody(11, iZone) = RealToStr(Zone(iZone).FloorArea * m2_unitConv / totNumPeople, 2);
                }
                // plug and process density
                totPlugProcess = 0.0;
                for (iPlugProc = 1; iPlugProc <= TotElecEquip; ++iPlugProc) {
                    if (iZone == ZoneElectric(iPlugProc).ZonePtr) {
                        totPlugProcess += ZoneElectric(iPlugProc).DesignLevel;
                    }
                }
                for (iPlugProc = 1; iPlugProc <= TotGasEquip; ++iPlugProc) {
                    if (iZone == ZoneGas(iPlugProc).ZonePtr) {
                        totPlugProcess += ZoneGas(iPlugProc).DesignLevel;
                    }
                }
                for (iPlugProc = 1; iPlugProc <= TotOthEquip; ++iPlugProc) {
                    if (iZone == ZoneOtherEq(iPlugProc).ZonePtr) {
                        totPlugProcess += ZoneOtherEq(iPlugProc).DesignLevel;
                    }
                }
                for (iPlugProc = 1; iPlugProc <= TotHWEquip; ++iPlugProc) {
                    if (iZone == ZoneHWEq(iPlugProc).ZonePtr) {
                        totPlugProcess += ZoneHWEq(iPlugProc).DesignLevel;
                    }
                }
                if (Zone(iZone).FloorArea > 0) {
                    tableBody(12, iZone) = RealToStr(totPlugProcess * Wm2_unitConv / Zone(iZone).FloorArea, 4);
                }

                // total rows for Total / Not Part of Total
                // In "Total": break between conditioned/unconditioned

                // If not part of total, goes directly to this row
                if (!usezoneFloorArea) {
                    zstArea(notpartTotal) += mult * Zone(iZone).FloorArea;
                    zstVolume(notpartTotal) += mult * Zone(iZone).Volume;
                    zstWallArea(notpartTotal) += mult * Zone(iZone).ExtGrossWallArea;
                    zstUndWallArea(notpartTotal) += mult * Zone(iZone).ExtGrossGroundWallArea;
                    zstWindowArea(notpartTotal) += mult * zoneGlassArea(iZone);
                    zstOpeningArea(notpartTotal) += mult * zoneOpeningArea(iZone);
                    zstLight(notpartTotal) += mult * totLightPower;
                    zstPeople(notpartTotal) += mult * totNumPeople;
                    zstPlug(notpartTotal) += mult * totPlugProcess;
                } else {
                    // Add it to the 'Total'
                    zstArea(grandTotal) += mult * Zone(iZone).FloorArea;
                    zstVolume(grandTotal) += mult * Zone(iZone).Volume;
                    zstWallArea(grandTotal) += mult * Zone(iZone).ExtGrossWallArea;
                    zstUndWallArea(grandTotal) += mult * Zone(iZone).ExtGrossGroundWallArea;
                    zstWindowArea(grandTotal) += mult * zoneGlassArea(iZone);
                    zstOpeningArea(grandTotal) += mult * zoneOpeningArea(iZone);
                    zstLight(grandTotal) += mult * totLightPower;
                    zstPeople(grandTotal) += mult * totNumPeople;
                    zstPlug(grandTotal) += mult * totPlugProcess;

                    // Subtotal between cond/unconditioned
                    if (zoneIsCond) {
                        zstArea(condTotal) += mult * Zone(iZone).FloorArea;
                        zstVolume(condTotal) += mult * Zone(iZone).Volume;
                        zstWallArea(condTotal) += mult * Zone(iZone).ExtGrossWallArea;
                        zstUndWallArea(condTotal) += mult * Zone(iZone).ExtGrossGroundWallArea;
                        zstWindowArea(condTotal) += mult * zoneGlassArea(iZone);
                        zstOpeningArea(condTotal) += mult * zoneOpeningArea(iZone);
                        zstLight(condTotal) += mult * totLightPower;
                        zstPeople(condTotal) += mult * totNumPeople;
                        zstPlug(condTotal) += mult * totPlugProcess;
                    } else if (!zoneIsCond) {
                        zstArea(uncondTotal) += mult * Zone(iZone).FloorArea;
                        zstVolume(uncondTotal) += mult * Zone(iZone).Volume;
                        zstWallArea(uncondTotal) += mult * Zone(iZone).ExtGrossWallArea;
                        zstUndWallArea(uncondTotal) += mult * Zone(iZone).ExtGrossGroundWallArea;
                        zstWindowArea(uncondTotal) += mult * zoneGlassArea(iZone);
                        zstOpeningArea(uncondTotal) += mult * zoneOpeningArea(iZone);
                        zstLight(uncondTotal) += mult * totLightPower;
                        zstPeople(uncondTotal) += mult * totNumPeople;
                        zstPlug(uncondTotal) += mult * totPlugProcess;
                    }
                }
            }
            for (iTotal = 1; iTotal <= 4; ++iTotal) {
                tableBody(1, state.dataGlobal->NumOfZones + iTotal) = RealToStr(zstArea(iTotal) * m2_unitConv, 2);
                tableBody(4, state.dataGlobal->NumOfZones + iTotal) = RealToStr(zstVolume(iTotal) * m3_unitConv, 2);
                tableBody(6, state.dataGlobal->NumOfZones + iTotal) = RealToStr(zstWallArea(iTotal) * m2_unitConv, 2);
                tableBody(7, state.dataGlobal->NumOfZones + iTotal) = RealToStr(zstUndWallArea(iTotal) * m2_unitConv, 2);
                tableBody(8, state.dataGlobal->NumOfZones + iTotal) = RealToStr(zstWindowArea(iTotal) * m2_unitConv, 2);
                tableBody(9, state.dataGlobal->NumOfZones + iTotal) = RealToStr(zstOpeningArea(iTotal) * m2_unitConv, 2);
                if (zstArea(iTotal) != 0) {
                    tableBody(10, state.dataGlobal->NumOfZones + iTotal) = RealToStr(zstLight(iTotal) * Wm2_unitConv / zstArea(iTotal), 4);
                    tableBody(12, state.dataGlobal->NumOfZones + iTotal) = RealToStr(zstPlug(iTotal) * Wm2_unitConv / zstArea(iTotal), 4);
                }
                if (zstPeople(iTotal) != 0) {
                    tableBody(11, state.dataGlobal->NumOfZones + iTotal) = RealToStr(zstArea(iTotal) * m2_unitConv / zstPeople(iTotal), 2);
                }
            }
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedSutSpArea, "Totals", zstArea(grandTotal), 2);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedSutOcArea, "Totals", zstArea(condTotal), 2);
            PreDefTableEntry(state, state.dataOutRptPredefined->pdchLeedSutUnArea, "Totals", zstArea(uncondTotal), 2);

            WriteSubtitle(state, "Zone Summary");
            WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
            if (sqlite) {
                sqlite->createSQLiteTabularDataRecords(
                    tableBody, rowHead, columnHead, "InputVerificationandResultsSummary", "Entire Facility", "Zone Summary");
            }
            if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                    tableBody, rowHead, columnHead, "Input Verification and Results Summary", "Entire Facility", "Zone Summary");
            }
        }
    }

    void WriteAdaptiveComfortTable(EnergyPlusData &state)
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Tyler Hoyt
        //       DATE WRITTEN   August 2011
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // Writes summary table for adaptive comfort models. Tabulates
        // occupied hours not meeting comfort bounds for ASHRAE-55 and
        // CEN-15251 adaptive models.

        // METHODOLOGY EMPLOYED:

        // REFERENCES:

        // Using/Aliasing
        using DataHeatBalance::People;
        using DataHeatBalance::TotPeople;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:

        Array1D_string columnHead(5);
        Array1D_int columnWidth;
        Array1D_string rowHead;
        Array2D_string tableBody;
        int i;
        Array1D_int peopleInd; // Index the relevant people
        auto &ort(state.dataOutRptTab);

        // Should deallocate after writing table. - LKL

        if (ort->displayAdaptiveComfort && TotPeople > 0) {
            peopleInd.allocate(TotPeople);

            for (i = 1; i <= TotPeople; ++i) {
                if (People(i).AdaptiveASH55 || People(i).AdaptiveCEN15251) {
                    ++ort->numPeopleAdaptive;
                    peopleInd(ort->numPeopleAdaptive) = i;
                }
            }

            rowHead.allocate(ort->numPeopleAdaptive);
            tableBody.allocate(5, ort->numPeopleAdaptive);

            WriteReportHeaders(state, "Adaptive Comfort Summary", "Entire Facility", OutputProcessor::StoreType::Averaged);
            WriteSubtitle(state, "Time Not Meeting the Adaptive Comfort Models during Occupied Hours");

            columnWidth.allocate(5);
            columnWidth = 10;
            columnHead(1) = "ASHRAE55 90% Acceptability Limits [Hours]";
            columnHead(2) = "ASHRAE55 80% Acceptability Limits  [Hours]";
            columnHead(3) = "CEN15251 Category I Acceptability Limits [Hours]";
            columnHead(4) = "CEN15251 Category II Acceptability Limits [Hours]";
            columnHead(5) = "CEN15251 Category III Acceptability Limits [Hours]";

            tableBody = "";
            for (i = 1; i <= ort->numPeopleAdaptive; ++i) {
                rowHead(i) = People(i).Name;
                if (People(i).AdaptiveASH55) {
                    tableBody(1, i) = RealToStr(People(peopleInd(i)).TimeNotMetASH5590, 2);
                    tableBody(2, i) = RealToStr(People(peopleInd(i)).TimeNotMetASH5580, 2);
                }
                if (People(i).AdaptiveCEN15251) {
                    tableBody(3, i) = RealToStr(People(peopleInd(i)).TimeNotMetCEN15251CatI, 2);
                    tableBody(4, i) = RealToStr(People(peopleInd(i)).TimeNotMetCEN15251CatII, 2);
                    tableBody(5, i) = RealToStr(People(peopleInd(i)).TimeNotMetCEN15251CatIII, 2);
                }
            }

            WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
            if (sqlite) {
                sqlite->createSQLiteTabularDataRecords(tableBody, rowHead, columnHead, "AdaptiveComfortReport", "Entire Facility", "People Summary");
            }
            if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                    tableBody, rowHead, columnHead, "Adaptive Comfort Report", "Entire Facility", "People Summary");
            }
        }
    }

    void WriteResilienceBinsTable(EnergyPlusData &state,
                                  int const columnNum,
                                  std::vector<int> const &columnHead,
                                  Array1D<std::vector<Real64>> const &ZoneBins)
    {
        std::vector<Real64> columnMax(columnNum, 0);
        std::vector<Real64> columnMin(columnNum, 0);
        std::vector<Real64> columnSum(columnNum, 0);
        for (int j = 0; j < columnNum; j++) {
            columnMin[j] = ZoneBins(1)[j];
        }
        for (int i = 1; i <= state.dataGlobal->NumOfZones; ++i) {
            std::string ZoneName = Zone(i).Name;
            for (int j = 0; j < columnNum; j++) {
                Real64 curValue = ZoneBins(i)[j];
                if (curValue > columnMax[j]) columnMax[j] = curValue;
                if (curValue < columnMin[j]) columnMin[j] = curValue;
                columnSum[j] += curValue;
                PreDefTableEntry(state, columnHead[j], ZoneName, RealToStr(curValue, 2));
            }
        }
        for (int j = 0; j < columnNum; j++) {
            PreDefTableEntry(state, columnHead[j], "Min", RealToStr(columnMin[j], 2));
            PreDefTableEntry(state, columnHead[j], "Max", RealToStr(columnMax[j], 2));
            PreDefTableEntry(state, columnHead[j], "Average", RealToStr(columnSum[j] / state.dataGlobal->NumOfZones, 2));
            PreDefTableEntry(state, columnHead[j], "Sum", RealToStr(columnSum[j], 2));
        }

    }

    void WriteSETHoursTable(EnergyPlusData &state,
                            int const columnNum,
                            std::vector<int> const &columnHead,
                            Array1D<std::vector<Real64>> const &ZoneBins)
    {
        std::vector<Real64> columnMax(columnNum - 1, 0);
        std::vector<Real64> columnMin(columnNum - 1, 0);
        std::vector<Real64> columnSum(columnNum - 1, 0);
        for (int j = 0; j < columnNum - 1; j++) {
            columnMin[j] = ZoneBins(1)[j];
        }
        for (int i = 1; i <= state.dataGlobal->NumOfZones; ++i) {
            for (int j = 0; j < columnNum - 1; j++) {
                Real64 curValue = ZoneBins(i)[j];
                if (curValue > columnMax[j]) columnMax[j] = curValue;
                if (curValue < columnMin[j]) columnMin[j] = curValue;
                columnSum[j] += curValue;
                PreDefTableEntry(state, columnHead[j], Zone(i).Name, RealToStr(curValue, 2));
            }
            std::string startDateTime = DateToString(int(ZoneBins(i)[columnNum - 1]));
            PreDefTableEntry(state, columnHead[columnNum - 1], Zone(i).Name, startDateTime);
        }
        for (int j = 0; j < columnNum - 1; j++) {
            PreDefTableEntry(state, columnHead[j], "Min", RealToStr(columnMin[j], 2));
            PreDefTableEntry(state, columnHead[j], "Max", RealToStr(columnMax[j], 2));
            PreDefTableEntry(state, columnHead[j], "Average", RealToStr(columnSum[j] / state.dataGlobal->NumOfZones, 2));
        }
        PreDefTableEntry(state, columnHead[columnNum - 1], "Min", "-");
        PreDefTableEntry(state, columnHead[columnNum - 1], "Max", "-");
        PreDefTableEntry(state, columnHead[columnNum - 1], "Average", "-");
    }

    void WriteThermalResilienceTables(EnergyPlusData &state)
    {

        // Using/Aliasing
        using DataHeatBalFanSys::ZoneHeatIndexHourBins;
        using DataHeatBalFanSys::ZoneHeatIndexOccuHourBins;
        using DataHeatBalFanSys::ZoneHumidexHourBins;
        using DataHeatBalFanSys::ZoneHumidexOccuHourBins;
        using DataHeatBalFanSys::ZoneLowSETHours;
        using DataHeatBalFanSys::ZoneHighSETHours;
        auto &ort(state.dataOutRptTab);

        if (state.dataGlobal->NumOfZones > 0) {
            int columnNum = 5;
            std::vector<int> columnHead = {state.dataOutRptPredefined->pdchHIHourSafe,
                                           state.dataOutRptPredefined->pdchHIHourCaution,
                                           state.dataOutRptPredefined->pdchHIHourExtremeCaution,
                                           state.dataOutRptPredefined->pdchHIHourDanger,
                                           state.dataOutRptPredefined->pdchHIHourExtremeDanger};
            WriteResilienceBinsTable(state, columnNum, columnHead, ZoneHeatIndexHourBins);

            columnHead = {state.dataOutRptPredefined->pdchHIOccuHourSafe,
                          state.dataOutRptPredefined->pdchHIOccuHourCaution,
                          state.dataOutRptPredefined->pdchHIOccuHourExtremeCaution,
                          state.dataOutRptPredefined->pdchHIOccuHourDanger,
                          state.dataOutRptPredefined->pdchHIOccuHourExtremeDanger};
            WriteResilienceBinsTable(state, columnNum, columnHead, ZoneHeatIndexOccuHourBins);

            columnHead = {state.dataOutRptPredefined->pdchHumidexHourLittle,
                          state.dataOutRptPredefined->pdchHumidexHourSome,
                          state.dataOutRptPredefined->pdchHumidexHourGreat,
                          state.dataOutRptPredefined->pdchHumidexHourDanger,
                          state.dataOutRptPredefined->pdchHumidexHourStroke};
            WriteResilienceBinsTable(state, columnNum, columnHead, ZoneHumidexHourBins);

            columnHead = {state.dataOutRptPredefined->pdchHumidexOccuHourLittle,
                          state.dataOutRptPredefined->pdchHumidexOccuHourSome,
                          state.dataOutRptPredefined->pdchHumidexOccuHourGreat,
                          state.dataOutRptPredefined->pdchHumidexOccuHourDanger,
                          state.dataOutRptPredefined->pdchHumidexOccuHourStroke };
            WriteResilienceBinsTable(state, columnNum, columnHead, ZoneHumidexOccuHourBins);

            bool hasPierceSET = true;
            if (TotPeople == 0) {
                hasPierceSET = false;
                if (ort->displayThermalResilienceSummaryExplicitly) {
                    ShowWarningError(state, "Writing Annual Thermal Resilience Summary - SET Hours reports: "
                                     "Zone Thermal Comfort Pierce Model Standard Effective Temperature is required, "
                                     "but no People object is defined.");
                }
            }
            for (int iPeople = 1; iPeople <= TotPeople; ++iPeople) {
                if (!People(iPeople).Pierce) {
                    hasPierceSET = false;
                    if (ort->displayThermalResilienceSummaryExplicitly) {
                        ShowWarningError(state,  "Writing Annual Thermal Resilience Summary - SET Hours reports: "
                                          "Zone Thermal Comfort Pierce Model Standard Effective Temperature is required, "
                                          "but no Pierce model is defined in " + People(iPeople).Name + " object.");
                    }
                }
            }

            if (hasPierceSET) {
                columnNum = 4;
                columnHead = {state.dataOutRptPredefined->pdchHeatingSETHours,
                              state.dataOutRptPredefined->pdchHeatingSETOccuHours,
                              state.dataOutRptPredefined->pdchHeatingSETUnmetDuration,
                              state.dataOutRptPredefined->pdchHeatingSETUnmetTime};
                WriteSETHoursTable(state, columnNum, columnHead, ZoneLowSETHours);

                columnHead = {state.dataOutRptPredefined->pdchCoolingSETHours,
                              state.dataOutRptPredefined->pdchCoolingSETOccuHours,
                              state.dataOutRptPredefined->pdchCoolingSETUnmetDuration,
                              state.dataOutRptPredefined->pdchCoolingSETUnmetTime};
                WriteSETHoursTable(state, columnNum, columnHead, ZoneHighSETHours);
            }
        }
    }

    void WriteCO2ResilienceTables(EnergyPlusData &state)
    {

        // Using/Aliasing
        using DataHeatBalFanSys::ZoneCO2LevelHourBins;
        using DataHeatBalFanSys::ZoneCO2LevelOccuHourBins;
        if (state.dataGlobal->NumOfZones > 0) {
            int columnNum = 3;
            std::vector<int> columnHead = {state.dataOutRptPredefined->pdchCO2HourSafe,
                                           state.dataOutRptPredefined->pdchCO2HourCaution,
                                           state.dataOutRptPredefined->pdchCO2HourHazard};
            WriteResilienceBinsTable(state, columnNum, columnHead, ZoneCO2LevelHourBins);

            columnHead = {state.dataOutRptPredefined->pdchCO2OccuHourSafe,
                          state.dataOutRptPredefined->pdchCO2OccuHourCaution,
                          state.dataOutRptPredefined->pdchCO2OccuHourHazard};
            WriteResilienceBinsTable(state, columnNum, columnHead, ZoneCO2LevelOccuHourBins);
        }
    }

    void WriteVisualResilienceTables(EnergyPlusData &state)
    {

        // Using/Aliasing
        using DataHeatBalFanSys::ZoneLightingLevelHourBins;
        using DataHeatBalFanSys::ZoneLightingLevelOccuHourBins;

        for (int ZoneNum = 1; ZoneNum <= state.dataGlobal->NumOfZones; ++ZoneNum) {
            if (state.dataDaylightingData->ZoneDaylight(ZoneNum).DaylightMethod == DataDaylighting::iDaylightingMethod::NoDaylighting) {
                if (state.dataOutRptTab->displayVisualResilienceSummaryExplicitly) {
                    ShowWarningError(state, "Writing Annual Visual Resilience Summary - Lighting Level Hours reports: "
                                     "Zone Average Daylighting Reference Point Illuminance output is required, "
                                     "but no Daylight Method is defined in Zone:" + Zone(ZoneNum).Name);
                }
            }
        }

        if (state.dataGlobal->NumOfZones > 0) {
            int columnNum = 4;
            std::vector<int> columnHead = {state.dataOutRptPredefined->pdchIllumHourDark,
                                           state.dataOutRptPredefined->pdchIllumHourDim,
                                           state.dataOutRptPredefined->pdchIllumHourAdequate,
                                           state.dataOutRptPredefined->pdchIllumHourBright};
            WriteResilienceBinsTable(state, columnNum, columnHead, ZoneLightingLevelHourBins);

            columnHead = {state.dataOutRptPredefined->pdchIllumOccuHourDark,
                          state.dataOutRptPredefined->pdchIllumOccuHourDim,
                          state.dataOutRptPredefined->pdchIllumOccuHourAdequate,
                          state.dataOutRptPredefined->pdchIllumOccuHourBright};
            WriteResilienceBinsTable(state, columnNum, columnHead, ZoneLightingLevelOccuHourBins);
        }
    }

    void WriteHeatEmissionTable(EnergyPlusData &state)
    {

        Array1D_string columnHead(6);
        Array1D_int columnWidth;
        Array1D_string rowHead;
        Array2D_string tableBody;

        if (state.dataOutRptTab->displayHeatEmissionsSummary) {

            WriteReportHeaders(state, "Annual Heat Emissions Summary", "Entire Facility", OutputProcessor::StoreType::Averaged);
            WriteSubtitle(state, "Heat Emission by Components");

            columnWidth.allocate(6);
            columnWidth = 10;

            rowHead.allocate(1);
            tableBody.allocate(6, 1);

            rowHead(1) = "Heat Emissions [GJ]";
            columnHead(1) = "Envelope Convection";
            columnHead(2) = "Zone Exfiltration";
            columnHead(3) = "Zone Exhaust Air";
            columnHead(4) = "HVAC Relief Air";
            columnHead(5) = "HVAC Reject Heat";
            columnHead(6) = "Total";

            tableBody = "";
            tableBody(1, 1) = RealToStr(BuildingPreDefRep.emiEnvelopConv, 2);
            tableBody(2, 1) = RealToStr(BuildingPreDefRep.emiZoneExfiltration, 2);
            tableBody(3, 1) = RealToStr(BuildingPreDefRep.emiZoneExhaust, 2);
            tableBody(4, 1) = RealToStr(BuildingPreDefRep.emiHVACRelief, 2);
            tableBody(5, 1) = RealToStr(BuildingPreDefRep.emiHVACReject, 2);
            tableBody(6, 1) = RealToStr(BuildingPreDefRep.emiTotHeat, 2);

            WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
            if (sqlite) {
                sqlite->createSQLiteTabularDataRecords(
                    tableBody, rowHead, columnHead, "AnnualHeatEmissionsReport", "Entire Facility", "Annual Heat Emissions Summary");
            }
        }
    }

    void WritePredefinedTables(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   August 2006
        //       MODIFIED       January 2010, Kyle Benne; Added SQLite output
        //                      March 2010, Linda Lawrie; Modify SizingPeriod:DesignDay to convert column/humidity types
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Write out tables that have been predefined with data gathered
        //   throughout the program code.

        // METHODOLOGY EMPLOYED:
        //   Create arrays for the call to WriteTable and then call it.
        //   This is a generic routine to write a report with multiple
        //   subtables. The structure of the report are created in
        //   OutputReportPredefined which also includes a routine that
        //   builds up a tableEntry array which holds the data for the
        //   predefined reports.

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:

        // all arrays are in the format: (row, column)
        Array1D_string columnHead;
        Array1D_int columnWidth;
        Array1D_string rowHead;
        Array2D_string tableBody;
        Array1D_int rowToUnqObjName;
        Array1D_int colHeadToColTag;
        int curNumColumns;
        int curNumRows;
        int curColumn;
        Array1D_string uniqueObjectName;
        Array1D_bool useUniqueObjectName;
        int numUnqObjName;
        std::string curObjectName;
        int countRow;
        int countColumn;
        int found;
        int curColTagIndex;
        int curRowUnqObjIndex;
        int colCurrent(0);
        int rowCurrent(0);
        int iReportName;
        int kColumnTag;
        int lTableEntry;
        int mUnqObjNames;
        int nColHead;
        int oRowHead;
        std::string colTagWithSI;
        std::string curColTag;
        Array1D_int colUnitConv;
        int indexUnitConv;
        int columnUnitConv;
        std::string repTableTag;
        Real64 IPvalue;
        auto &ort(state.dataOutRptTab);

        // loop through the entries and associate them with the subtable and create
        // list of unique object names
        // Much of this code is to allow for integer compares instead of string
        // compares that are nested three levels in a loop.
        uniqueObjectName.allocate(state.dataOutRptPredefined->numTableEntry);
        useUniqueObjectName.allocate(state.dataOutRptPredefined->numTableEntry);
        numUnqObjName = 0;
        for (lTableEntry = 1; lTableEntry <= state.dataOutRptPredefined->numTableEntry; ++lTableEntry) {
            // associate the subtable with each column
            curColumn = state.dataOutRptPredefined->tableEntry(lTableEntry).indexColumn;
            if ((curColumn >= 1) && (curColumn <= state.dataOutRptPredefined->numColumnTag)) {
                state.dataOutRptPredefined->tableEntry(lTableEntry).subTableIndex = state.dataOutRptPredefined->columnTag(curColumn).indexSubTable;
            }
            // make a list of unique object names
            curObjectName = state.dataOutRptPredefined->tableEntry(lTableEntry).objectName;
            found = 0;
            for (mUnqObjNames = 1; mUnqObjNames <= numUnqObjName; ++mUnqObjNames) {
                if (curObjectName == uniqueObjectName(mUnqObjNames)) {
                    found = mUnqObjNames;
                }
            }
            // if found then point to the unique object
            if (found > 0) {
                state.dataOutRptPredefined->tableEntry(lTableEntry).uniqueObjName = found;
                // if not found add to the unique object list
            } else {
                ++numUnqObjName;
                uniqueObjectName(numUnqObjName) = curObjectName;
                state.dataOutRptPredefined->tableEntry(lTableEntry).uniqueObjName = numUnqObjName;
            }
        }
        // loop through all reports and include those that have been flagged as 'show'
        for (iReportName = 1; iReportName <= state.dataOutRptPredefined->numReportName; ++iReportName) {
            if (state.dataOutRptPredefined->reportName(iReportName).show) {
                WriteReportHeaders(state, state.dataOutRptPredefined->reportName(iReportName).namewithspaces, "Entire Facility", OutputProcessor::StoreType::Averaged);
                // loop through the subtables and include those that are associated with this report
                for (int jSubTable = 1, jSubTable_end = state.dataOutRptPredefined->numSubTable; jSubTable <= jSubTable_end; ++jSubTable) {
                    if (state.dataOutRptPredefined->subTable(jSubTable).indexReportName == iReportName) {
                        // determine how many columns
                        curNumColumns = 0;
                        for (kColumnTag = 1; kColumnTag <= state.dataOutRptPredefined->numColumnTag; ++kColumnTag) {
                            if (state.dataOutRptPredefined->columnTag(kColumnTag).indexSubTable == jSubTable) {
                                ++curNumColumns;
                            }
                        }
                        // determine how many rows by going through table entries and setting
                        // flag in useUniqueObjectName to true, then count number of true's.
                        useUniqueObjectName = false; // array assignment
                        for (lTableEntry = 1; lTableEntry <= state.dataOutRptPredefined->numTableEntry; ++lTableEntry) {
                            if (state.dataOutRptPredefined->tableEntry(lTableEntry).subTableIndex == jSubTable) {
                                useUniqueObjectName(state.dataOutRptPredefined->tableEntry(lTableEntry).uniqueObjName) = true;
                            }
                        }
                        curNumRows = 0;
                        for (mUnqObjNames = 1; mUnqObjNames <= numUnqObjName; ++mUnqObjNames) {
                            if (useUniqueObjectName(mUnqObjNames)) {
                                ++curNumRows;
                            }
                        }
                        if (curNumRows == 0) curNumRows = 1;
                        // now create the arrays that are filled with values
                        rowHead.allocate(curNumRows);
                        columnHead.allocate(curNumColumns);
                        columnWidth.dimension(curNumColumns, 14); // array assignment - same for all columns
                        tableBody.allocate(curNumColumns, curNumRows);
                        rowHead = "";
                        columnHead = "";
                        tableBody = "";
                        // this array stores the unique object name index for each row
                        rowToUnqObjName.allocate(curNumRows);
                        // this array stores the columnHead index for each column
                        colHeadToColTag.allocate(curNumColumns);
                        colUnitConv.allocate(curNumColumns);
                        // set row headings
                        countRow = 0;
                        rowHead(1) = "None";
                        for (mUnqObjNames = 1; mUnqObjNames <= numUnqObjName; ++mUnqObjNames) {
                            if (useUniqueObjectName(mUnqObjNames)) {
                                ++countRow;
                                rowHead(countRow) = uniqueObjectName(mUnqObjNames);
                                rowToUnqObjName(countRow) = mUnqObjNames;
                            }
                        }
                        // set column headings
                        countColumn = 0;
                        for (kColumnTag = 1; kColumnTag <= state.dataOutRptPredefined->numColumnTag; ++kColumnTag) {
                            if (state.dataOutRptPredefined->columnTag(kColumnTag).indexSubTable == jSubTable) {
                                ++countColumn;
                                // do the unit conversions
                                colTagWithSI = state.dataOutRptPredefined->columnTag(kColumnTag).heading;
                                if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                    LookupSItoIP(state, colTagWithSI, indexUnitConv, curColTag);
                                    colUnitConv(countColumn) = indexUnitConv;
                                } else if (ort->unitsStyle == iUnitsStyle::JtoKWH) {
                                    LookupJtokWH(state, colTagWithSI, indexUnitConv, curColTag);
                                    colUnitConv(countColumn) = indexUnitConv;
                                } else {
                                    curColTag = colTagWithSI;
                                    colUnitConv(countColumn) = 0;
                                }
                                columnHead(countColumn) = curColTag;
                                colHeadToColTag(countColumn) = kColumnTag;
                            }
                        }
                        // fill the body of the table from the entries
                        // find the entries associated with the current subtable
                        for (lTableEntry = 1; lTableEntry <= state.dataOutRptPredefined->numTableEntry; ++lTableEntry) {
                            if (state.dataOutRptPredefined->tableEntry(lTableEntry).subTableIndex == jSubTable) {
                                // determine what column the current entry is in
                                curColTagIndex = state.dataOutRptPredefined->tableEntry(lTableEntry).indexColumn;
                                for (nColHead = 1; nColHead <= curNumColumns; ++nColHead) {
                                    if (curColTagIndex == colHeadToColTag(nColHead)) {
                                        colCurrent = nColHead;
                                        break;
                                    }
                                }
                                // determine what row the current entry is in
                                curRowUnqObjIndex = state.dataOutRptPredefined->tableEntry(lTableEntry).uniqueObjName;
                                for (oRowHead = 1; oRowHead <= curNumRows; ++oRowHead) {
                                    if (curRowUnqObjIndex == rowToUnqObjName(oRowHead)) {
                                        rowCurrent = oRowHead;
                                        break;
                                    }
                                }
                                // finally assign the entry to the place in the table body
                                if (ort->unitsStyle == iUnitsStyle::InchPound || ort->unitsStyle == iUnitsStyle::JtoKWH) {
                                    columnUnitConv = colUnitConv(colCurrent);
                                    if (UtilityRoutines::SameString(state.dataOutRptPredefined->subTable(jSubTable).name, "SizingPeriod:DesignDay") &&
                                        ort->unitsStyle == iUnitsStyle::InchPound) {
                                        if (UtilityRoutines::SameString(columnHead(colCurrent), "Humidity Value")) {
                                            LookupSItoIP(state, state.dataOutRptPredefined->tableEntry(lTableEntry + 1).charEntry, columnUnitConv, repTableTag);
                                            state.dataOutRptPredefined->tableEntry(lTableEntry + 1).charEntry = repTableTag;
                                        }
                                    }
                                    if (state.dataOutRptPredefined->tableEntry(lTableEntry).origEntryIsReal && (columnUnitConv != 0)) {
                                        IPvalue = ConvertIP(state, columnUnitConv, state.dataOutRptPredefined->tableEntry(lTableEntry).origRealEntry);
                                        tableBody(colCurrent, rowCurrent) = RealToStr(IPvalue, state.dataOutRptPredefined->tableEntry(lTableEntry).significantDigits);
                                    } else {
                                        tableBody(colCurrent, rowCurrent) = state.dataOutRptPredefined->tableEntry(lTableEntry).charEntry;
                                    }
                                } else {
                                    tableBody(colCurrent, rowCurrent) = state.dataOutRptPredefined->tableEntry(lTableEntry).charEntry;
                                }
                            }
                        }
                        // create the actual output table
                        WriteSubtitle(state, state.dataOutRptPredefined->subTable(jSubTable).name);
                        WriteTable(state, tableBody, rowHead, columnHead, columnWidth, false, state.dataOutRptPredefined->subTable(jSubTable).footnote);
                        if (sqlite) {
                            sqlite->createSQLiteTabularDataRecords(
                                tableBody, rowHead, columnHead, state.dataOutRptPredefined->reportName(iReportName).name, "Entire Facility", state.dataOutRptPredefined->subTable(jSubTable).name);
                        }
                        if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                            ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                                tableBody, rowHead, columnHead, state.dataOutRptPredefined->reportName(iReportName).name, "Entire Facility", state.dataOutRptPredefined->subTable(jSubTable).name);
                        }
                    }
                }
            }
        }
    }

    void WriteComponentSizing(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   July 2007
        //       MODIFIED       January 2010, Kyle Benne
        //                      Added SQLite output
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Write out tables based on component sizing data originally
        //   found in the EIO report.

        // METHODOLOGY EMPLOYED:
        //   Create arrays for the call to WriteTable and then call it.
        //   The tables created do not have known headers for rows or
        //   columns so those are determined based on what calls have
        //   been made to the Sizer routine.  A table
        //   is created for each type of component. Columns are created
        //   for each description within that table. Rows are created
        //   for each named object.

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        // all arrays are in the format: (row, column)
        Array1D_string columnHead;
        Array1D_int columnWidth;
        Array1D_int colUnitConv;
        Array1D_string rowHead;
        Array2D_string tableBody;
        Array1D_string uniqueDesc;
        int numUniqueDesc;
        Array1D_string uniqueObj;
        int numUniqueObj;
        std::string curDesc;
        std::string curObj;
        int foundEntry;
        int foundDesc;
        int foundObj;
        int loopLimit;
        int iTableEntry;
        int jUnique;
        static std::string curColHeadWithSI;
        static std::string curColHead;
        static int indexUnitConv(0);
        static Real64 curValueSI(0.0);
        static Real64 curValue(0.0);
        auto &ort(state.dataOutRptTab);

        if (ort->displayComponentSizing) {
            WriteReportHeaders(state, "Component Sizing Summary", "Entire Facility", OutputProcessor::StoreType::Averaged);
            // The arrays that look for unique headers are dimensioned in the
            // running program since the size of the number of entries is
            // not previouslly known. Use the size of all entries since that
            // is the maximum possible.
            uniqueDesc.allocate(state.dataOutRptPredefined->numCompSizeTableEntry);
            uniqueObj.allocate(state.dataOutRptPredefined->numCompSizeTableEntry);
            // initially clear the written flags for entire array
            // The following line is not really necessary and it is possible that the array has
            // not been allocated when this is first called.
            //  CompSizeTableEntry%written = .FALSE.
            // repeat the following loop until everything in array has been
            // written into a table
            loopLimit = 0;
            while (loopLimit <= 100) { // put a maximum count since complex loop that could run indefinitely if error
                foundEntry = 0;
                ++loopLimit;
                for (iTableEntry = 1; iTableEntry <= state.dataOutRptPredefined->numCompSizeTableEntry; ++iTableEntry) {
                    if (!state.dataOutRptPredefined->CompSizeTableEntry(iTableEntry).written) {
                        foundEntry = iTableEntry;
                        break;
                    }
                }
                if (foundEntry == 0) break; // leave main loop - all items put into tables
                // clear active items
                for (auto &e : state.dataOutRptPredefined->CompSizeTableEntry)
                    e.active = false;
                // make an unwritten item that is of the same type active - these will be the
                // entries for the particular subtable.
                for (iTableEntry = 1; iTableEntry <= state.dataOutRptPredefined->numCompSizeTableEntry; ++iTableEntry) {
                    if (!state.dataOutRptPredefined->CompSizeTableEntry(iTableEntry).written) {
                        if (UtilityRoutines::SameString(state.dataOutRptPredefined->CompSizeTableEntry(iTableEntry).typeField, state.dataOutRptPredefined->CompSizeTableEntry(foundEntry).typeField)) {
                            state.dataOutRptPredefined->CompSizeTableEntry(iTableEntry).active = true;
                        }
                    }
                }
                // identify unique descriptions and objects (columns and rows) in order
                // to size the table arrays properly.
                // reset the counters for the arrays looking for unique rows and columns
                numUniqueDesc = 0;
                numUniqueObj = 0;
                for (iTableEntry = 1; iTableEntry <= state.dataOutRptPredefined->numCompSizeTableEntry; ++iTableEntry) {
                    // search for descriptions
                    foundDesc = 0;
                    if (state.dataOutRptPredefined->CompSizeTableEntry(iTableEntry).active) {
                        curDesc = state.dataOutRptPredefined->CompSizeTableEntry(iTableEntry).description;
                        // look through the list of unique items to see if it matches
                        for (jUnique = 1; jUnique <= numUniqueDesc; ++jUnique) {
                            if (UtilityRoutines::SameString(curDesc, uniqueDesc(jUnique))) {
                                foundDesc = jUnique;
                                break;
                            }
                        }
                        // if not found add to the list
                        if (foundDesc == 0) {
                            ++numUniqueDesc;
                            uniqueDesc(numUniqueDesc) = curDesc;
                        }
                        // search for objects
                        foundObj = 0;
                        curObj = state.dataOutRptPredefined->CompSizeTableEntry(iTableEntry).nameField;
                        for (jUnique = 1; jUnique <= numUniqueObj; ++jUnique) {
                            if (UtilityRoutines::SameString(curObj, uniqueObj(jUnique))) {
                                foundObj = jUnique;
                                break;
                            }
                        }
                        // if not found add to the list
                        if (foundObj == 0) {
                            ++numUniqueObj;
                            uniqueObj(numUniqueObj) = curObj;
                        }
                    }
                }
                // make sure the table has at least one row and columns
                if (numUniqueDesc == 0) numUniqueDesc = 1;
                if (numUniqueObj == 0) numUniqueObj = 1;
                // now that the unique row and column headers are known the array
                // sizes can be set for the table arrays
                rowHead.allocate(numUniqueObj);
                columnHead.allocate(numUniqueDesc);
                columnWidth.dimension(numUniqueDesc, 14); // array assignment - same for all columns
                colUnitConv.allocate(numUniqueDesc);
                tableBody.allocate(numUniqueDesc, numUniqueObj);
                // initialize table body to blanks (in case entries are incomplete)
                tableBody = "";
                // transfer the row and column headings first
                for (jUnique = 1; jUnique <= numUniqueDesc; ++jUnique) {
                    // do the unit conversions
                    curColHeadWithSI = uniqueDesc(jUnique);
                    if (ort->unitsStyle == iUnitsStyle::InchPound) {
                        LookupSItoIP(state, curColHeadWithSI, indexUnitConv, curColHead);
                        colUnitConv(jUnique) = indexUnitConv;
                    } else {
                        curColHead = curColHeadWithSI;
                        colUnitConv(jUnique) = 0;
                    }
                    columnHead(jUnique) = curColHead;
                }
                for (jUnique = 1; jUnique <= numUniqueObj; ++jUnique) {
                    rowHead(jUnique) = uniqueObj(jUnique);
                }
                // fill the table
                for (iTableEntry = 1; iTableEntry <= state.dataOutRptPredefined->numCompSizeTableEntry; ++iTableEntry) {
                    // find the row and column for the specific entry
                    if (state.dataOutRptPredefined->CompSizeTableEntry(iTableEntry).active) {
                        curDesc = state.dataOutRptPredefined->CompSizeTableEntry(iTableEntry).description;
                        foundDesc = 0;
                        for (jUnique = 1; jUnique <= numUniqueDesc; ++jUnique) {
                            if (UtilityRoutines::SameString(uniqueDesc(jUnique), curDesc)) {
                                foundDesc = jUnique;
                                break;
                            }
                        }
                        curObj = state.dataOutRptPredefined->CompSizeTableEntry(iTableEntry).nameField;
                        foundObj = 0;
                        for (jUnique = 1; jUnique <= numUniqueObj; ++jUnique) {
                            if (UtilityRoutines::SameString(rowHead(jUnique), curObj)) {
                                foundObj = jUnique;
                                break;
                            }
                        }
                        if ((foundDesc >= 1) && (foundObj >= 1)) {
                            curValueSI = state.dataOutRptPredefined->CompSizeTableEntry(iTableEntry).valField;
                            if (ort->unitsStyle == iUnitsStyle::InchPound) {
                                if (colUnitConv(foundDesc) != 0) {
                                    curValue = ConvertIP(state, colUnitConv(foundDesc), curValueSI);
                                } else {
                                    curValue = curValueSI;
                                }
                            } else {
                                curValue = curValueSI;
                            }
                            if (std::abs(curValue) >= 1.0) {
                                tableBody(foundDesc, foundObj) = RealToStr(curValue, 2);
                            } else {
                                tableBody(foundDesc, foundObj) = RealToStr(curValue, 6);
                            }
                            state.dataOutRptPredefined->CompSizeTableEntry(iTableEntry).written = true;
                        }
                    }
                }
                // write the table
                WriteSubtitle(state, state.dataOutRptPredefined->CompSizeTableEntry(foundEntry).typeField);
                if (state.dataOutRptPredefined->CompSizeTableEntry(foundEntry).typeField == "AirTerminal:SingleDuct:VAV:Reheat" ||
                    state.dataOutRptPredefined->CompSizeTableEntry(foundEntry).typeField == "AirTerminal:SingleDuct:VAV:NoReheat") {
                    WriteTable(state, tableBody,
                               rowHead,
                               columnHead,
                               columnWidth,
                               false,
                               "User-Specified values were used. Design Size values were used if no User-Specified values were provided. Design Size "
                               "values may be derived from alternate User-Specified values.");
                } else {
                    WriteTable(state, tableBody,
                               rowHead,
                               columnHead,
                               columnWidth,
                               false,
                               "User-Specified values were used. Design Size values were used if no User-Specified values were provided.");
                }
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(
                        tableBody, rowHead, columnHead, "ComponentSizingSummary", "Entire Facility", state.dataOutRptPredefined->CompSizeTableEntry(foundEntry).typeField);
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                        tableBody,
                        rowHead,
                        columnHead,
                        "Component Sizing Summary",
                        "Entire Facility",
                        state.dataOutRptPredefined->CompSizeTableEntry(foundEntry).typeField,
                        "User-Specified values were used. Design Size values were used if no User-Specified values were provided.");
                }
            }
        }
    }

    void WriteSurfaceShadowing(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   July 2007
        //       MODIFIED       January 2010, Kyle Benne
        //                      Added SQLite output
        //       RE-ENGINEERED  June 2014, Stuart Mentzer, Performance tuning

        // PURPOSE OF THIS SUBROUTINE:
        //   Write out tables based on which surfaces shade subsurfaces.

        // METHODOLOGY EMPLOYED:
        //   Create arrays for the call to WriteTable and then call it.
        //   Use <br> tag to put multiple rows into a single cell.

        // REFERENCES:
        // na

        // Using/Aliasing
        using DataSurfaces::Surface;
        using DataSurfaces::TotSurfaces;
        using namespace DataShadowingCombinations;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        // all arrays are in the format: (row, column)
        Array1D_string columnHead(1);
        Array1D_int columnWidth(1);
        Array1D_string rowHead;
        Array2D_string tableBody;
        Array1D_int unique;
        int numUnique;
        int curRecSurf;
        std::string listOfSurf;
        int iShadRel;
        int jUnique;
        int iKindRec;
        int numreceivingfields;
        int HTS;
        int NGSS;
        auto &ort(state.dataOutRptTab);

        // displaySurfaceShadowing = false  for debugging
        if (ort->displaySurfaceShadowing) {
            numreceivingfields = 0;
            for (HTS = 1; HTS <= TotSurfaces; ++HTS) {
                numreceivingfields += ShadowComb(HTS).NumGenSurf;
                numreceivingfields += ShadowComb(HTS).NumSubSurf;
            }

            state.dataOutRptPredefined->ShadowRelate.allocate(numreceivingfields);
            state.dataOutRptPredefined->numShadowRelate = 0;
            for (HTS = 1; HTS <= TotSurfaces; ++HTS) {
                for (NGSS = 1; NGSS <= ShadowComb(HTS).NumGenSurf; ++NGSS) {
                    ++state.dataOutRptPredefined->numShadowRelate;
                    state.dataOutRptPredefined->ShadowRelate(state.dataOutRptPredefined->numShadowRelate).castSurf = ShadowComb(HTS).GenSurf(NGSS);
                    state.dataOutRptPredefined->ShadowRelate(state.dataOutRptPredefined->numShadowRelate).recSurf = HTS;
                    state.dataOutRptPredefined->ShadowRelate(state.dataOutRptPredefined->numShadowRelate).recKind = recKindSurface;
                }
                for (NGSS = 1; NGSS <= ShadowComb(HTS).NumSubSurf; ++NGSS) {
                    ++state.dataOutRptPredefined->numShadowRelate;
                    state.dataOutRptPredefined->ShadowRelate(state.dataOutRptPredefined->numShadowRelate).castSurf = ShadowComb(HTS).SubSurf(NGSS);
                    state.dataOutRptPredefined->ShadowRelate(state.dataOutRptPredefined->numShadowRelate).recSurf = HTS;
                    state.dataOutRptPredefined->ShadowRelate(state.dataOutRptPredefined->numShadowRelate).recKind = recKindSubsurface;
                }
            }
            assert(numreceivingfields == state.dataOutRptPredefined->numShadowRelate);

            WriteReportHeaders(state, "Surface Shadowing Summary", "Entire Facility", OutputProcessor::StoreType::Averaged);
            unique.allocate(state.dataOutRptPredefined->numShadowRelate);
            // do entire process twice, once with surfaces receiving, once with subsurfaces receiving
            for (iKindRec = recKindSurface; iKindRec <= recKindSubsurface; ++iKindRec) {

                // Build map from receiving surface to container of names
                typedef std::map<int, std::pair<int, std::vector<std::string const *>>> ShadowMap;
                ShadowMap shadow_map;
                for (iShadRel = 1; iShadRel <= state.dataOutRptPredefined->numShadowRelate; ++iShadRel) {
                    if (state.dataOutRptPredefined->ShadowRelate(iShadRel).recKind == iKindRec) {
                        curRecSurf = state.dataOutRptPredefined->ShadowRelate(iShadRel).recSurf;
                        std::string const &name(Surface(state.dataOutRptPredefined->ShadowRelate(iShadRel).castSurf).Name);
                        auto &elem(shadow_map[curRecSurf]);            // Creates the entry if not present (and zero-initializes the int in the pair)
                        elem.first += static_cast<int>(name.length()); // Accumulate total of name lengths
                        elem.second.push_back(&name);                  // Add this name
                    }
                }
                numUnique = static_cast<int>(shadow_map.size());
                if (numUnique == 0) {
                    columnHead(1) = "None";
                } else {
                    columnHead(1) = "Possible Shadow Receivers";
                }
                columnWidth = 14; // array assignment - same for all columns
                rowHead.allocate(numUnique);
                tableBody.allocate(1, numUnique);
                jUnique = 0;
                for (auto const &elem : shadow_map) {
                    ++jUnique;
                    curRecSurf = elem.first;
                    rowHead(jUnique) = Surface(curRecSurf).Name;
                    listOfSurf.clear();
                    listOfSurf.reserve(elem.second.first + (3 * numUnique)); // To avoid string allocations during appends
                    for (auto const *p : elem.second.second) {
                        listOfSurf += *p;
                        listOfSurf += " | "; //'<br>' // Separate append to avoid string temporary
                    }
                    tableBody(1, jUnique) = listOfSurf;
                }

                // write the table
                if (iKindRec == recKindSurface) {
                    WriteSubtitle(state, "Surfaces (Walls, Roofs, etc) that may be Shadowed by Other Surfaces");
                    if (sqlite) {
                        sqlite->createSQLiteTabularDataRecords(tableBody,
                                                               rowHead,
                                                               columnHead,
                                                               "SurfaceShadowingSummary",
                                                               "Entire Facility",
                                                               "Surfaces (Walls, Roofs, etc) that may be Shadowed by Other Surfaces");
                    }
                    if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                        ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                            tableBody,
                            rowHead,
                            columnHead,
                            "Surface Shadowing Summary",
                            "Entire Facility",
                            "Surfaces (Walls, Roofs, etc) that may be Shadowed by Other Surfaces");
                    }
                } else if (iKindRec == recKindSubsurface) {
                    WriteSubtitle(state, "Subsurfaces (Windows and Doors) that may be Shadowed by Surfaces");
                    if (sqlite) {
                        sqlite->createSQLiteTabularDataRecords(tableBody,
                                                               rowHead,
                                                               columnHead,
                                                               "SurfaceShadowingSummary",
                                                               "Entire Facility",
                                                               "Subsurfaces (Windows and Doors) that may be Shadowed by Surfaces");
                    }
                    if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                        ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                            tableBody,
                            rowHead,
                            columnHead,
                            "Surface Shadowing Summary",
                            "Entire Facility",
                            "Subsurfaces (Windows and Doors) that may be Shadowed by Surfaces");
                    }
                }
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
            }
        }
    }

    // Parses the contents of the EIO (initializations) file and creates subtables for each type of record in the tabular output files
    // Glazer - November 2016
    void WriteEioTables(EnergyPlusData &state)
    {

        auto &ort(state.dataOutRptTab);

        if (ort->displayEioSummary) {
            Array1D_string columnHead;
            Array1D_int columnWidth;
            Array1D_string rowHead;
            Array2D_string tableBody; // in the format: (row, column)
            Array1D_int colUnitConv;

            // setting up  report header
            WriteReportHeaders(state, "Initialization Summary", "Entire Facility", OutputProcessor::StoreType::Averaged);

            std::vector<std::string> headerLines; // holds the lines that describe each type of records - each starts with ! symbol
            std::vector<std::string> bodyLines;   // holds the data records only
            for (auto const &line : state.files.eio.getLines()) {
                if (line.at(0) == '!') {
                    headerLines.push_back(line);
                } else {
                    if (line.at(0) == ' ') {
                        bodyLines.push_back(line.substr(1)); // remove leading space
                    } else {
                        bodyLines.push_back(line);
                    }
                }
            }

            // now go through each header and create a report for each one
            for (auto headerLine : headerLines) {
                std::vector<std::string> headerFields = splitCommaString(headerLine);
                std::string tableNameWithSigns = headerFields.at(0);
                std::string tableName =
                    tableNameWithSigns.substr(3, tableNameWithSigns.size() - 4); // get rid of the '! <' from the beginning and the '>' from the end
                // first count the number of matching lines
                int countOfMatchingLines = 0;
                for (auto bodyLine : bodyLines) {
                    if (bodyLine.size() > tableName.size()) {
                        if (bodyLine.substr(0, tableName.size() + 1) ==
                            tableName + ",") { // this needs to match the test used to populate the body of table below
                            ++countOfMatchingLines;
                        }
                    }
                }
                int numRows = countOfMatchingLines;
                int numCols = headerFields.size() - 1;

                if (numRows >= 1) {
                    rowHead.allocate(numRows);
                    columnHead.allocate(numCols);
                    columnWidth.allocate(numCols);
                    columnWidth = 14; // array assignment - same for all columns
                    tableBody.allocate(numCols, numRows);
                    tableBody = ""; // make sure everything is blank
                    std::string footnote = "";
                    colUnitConv.allocate(numCols);
                    // transfer the header row into column headings
                    for (int iCol = 1; iCol <= numCols; ++iCol) {
                        columnHead(iCol) = headerFields.at(iCol);
                        // set the unit conversions
                        colUnitConv(iCol) = unitsFromHeading(state, columnHead(iCol));
                    }
                    // look for data lines
                    int rowNum = 0;
                    for (auto bodyLine : bodyLines) {
                        if (bodyLine.size() > tableName.size()) {
                            if (bodyLine.substr(0, tableName.size() + 1) ==
                                tableName + ",") { // this needs to match the test used in the original counting
                                ++rowNum;
                                if (rowNum > countOfMatchingLines) break; // should never happen since same test as original could
                                std::vector<std::string> dataFields = splitCommaString(bodyLine);
                                rowHead(rowNum) = fmt::to_string(rowNum);
                                for (int iCol = 1; iCol <= numCols && iCol < int(dataFields.size()); ++iCol) {
                                    if (ort->unitsStyle == iUnitsStyle::InchPound || ort->unitsStyle == iUnitsStyle::JtoKWH) {
                                        if (isNumber(dataFields[iCol]) && colUnitConv(iCol) > 0) { // if it is a number that has a conversion
                                            int numDecimalDigits = digitsAferDecimal(dataFields[iCol]);
                                            Real64 convertedVal = ConvertIP(state, colUnitConv(iCol), StrToReal(dataFields[iCol]));
                                            tableBody(iCol, rowNum) = RealToStr(convertedVal, numDecimalDigits);
                                        } else if (iCol == numCols && columnHead(iCol) == "Value" && iCol > 1) { // if it is the last column and the
                                                                                                                 // header is Value then treat the
                                                                                                                 // previous column as source of units
                                            int indexUnitConv = unitsFromHeading(state, tableBody(iCol - 1, rowNum));   // base units on previous column
                                            int numDecimalDigits = digitsAferDecimal(dataFields[iCol]);
                                            Real64 convertedVal = ConvertIP(state, indexUnitConv, StrToReal(dataFields[iCol]));
                                            tableBody(iCol, rowNum) = RealToStr(convertedVal, numDecimalDigits);
                                        } else {
                                            tableBody(iCol, rowNum) = dataFields[iCol];
                                        }
                                    } else {
                                        tableBody(iCol, rowNum) = dataFields[iCol];
                                    }
                                }
                            }
                        }
                    }

                    WriteSubtitle(state, tableName);
                    WriteTable(state, tableBody, rowHead, columnHead, columnWidth, false, footnote);
                    if (sqlite) {
                        sqlite->createSQLiteTabularDataRecords(
                            tableBody, rowHead, columnHead, "Initialization Summary", "Entire Facility", tableName);
                    }
                }
            }

            // as of Oct 2016 only the <Program Control Information:Threads/Parallel Sims> section is written after this point
        }
    }

    // changes the heading that contains and SI to IP as well as providing the unit conversion index
    // Glazer Nov 2016
    int unitsFromHeading(EnergyPlusData &state, std::string &heading)
    {
        auto &ort(state.dataOutRptTab);

        std::string curHeading = "";
        int unitConv = 0;
        if (ort->unitsStyle == iUnitsStyle::InchPound) {
            LookupSItoIP(state, heading, unitConv, curHeading);
        } else if (ort->unitsStyle == iUnitsStyle::JtoKWH) {
            LookupJtokWH(state, heading, unitConv, curHeading);
        } else {
            curHeading = heading;
        }
        heading = curHeading;
        return (unitConv);
    }

    // function that returns a vector of strings when given a string with comma delimitters
    // Glazer Nov 2016
    std::vector<std::string> splitCommaString(std::string const &inputString)
    {
        std::vector<std::string> fields;
        std::string field;
        std::stringstream inputSS(inputString);
        while (std::getline(inputSS, field, ',')) {
            fields.push_back(stripped(field));
        }
        return fields;
    }

    void AddTOCLoadComponentTableSummaries(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   March 2012
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Add the table of contents entries for the Zone heat transfer
        //   summary report.

        // METHODOLOGY EMPLOYED:
        //   Call the AddTOCEntry routine for each zone.

        // REFERENCES:
        // na

        // USE STATEMENTS:
        // na

        // Using/Aliasing
        using DataHeatBalance::Zone;
        using DataZoneEquipment::ZoneEquipConfig;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:

        int iZone;
        auto &ort(state.dataOutRptTab);

        if (state.dataGlobal->CompLoadReportIsReq) {
            if (ort->displayZoneComponentLoadSummary) {
                for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
                    if (!ZoneEquipConfig(iZone).IsControlled) continue;
                    AddTOCEntry(state, "Zone Component Load Summary", Zone(iZone).Name);
                }
            }
            if (ort->displayAirLoopComponentLoadSummary) {
                for (int AirLoopNum = 1; AirLoopNum <= DataHVACGlobals::NumPrimaryAirSys; ++AirLoopNum) {
                    AddTOCEntry(state, "AirLoop Component Load Summary", DataSizing::FinalSysSizing(AirLoopNum).AirPriLoopName);
                }
            }
            if (ort->displayFacilityComponentLoadSummary) {
                AddTOCEntry(state, "Facility Component Load Summary", "Facility");
            }
        }
    }

    void AllocateLoadComponentArrays(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   April 2012
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Allocate the arrays related to the load component report

        // METHODOLOGY EMPLOYED:
        //   Use the ALLOCATE command

        // REFERENCES:
        // na

        // USE STATEMENTS:
        // na
        // Using/Aliasing
        using DataSurfaces::TotSurfaces;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        auto &ort(state.dataOutRptTab);

        if (ort->AllocateLoadComponentArraysDoAllocate) {
            // For many of the following arrays the last dimension is the number of environments and is same as sizing arrays
            ort->radiantPulseTimestep.allocate({0, state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays}, state.dataGlobal->NumOfZones);
            ort->radiantPulseTimestep = 0;
            ort->radiantPulseReceived.allocate({0, state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays}, TotSurfaces);
            ort->radiantPulseReceived = 0.0;
            ort->loadConvectedNormal.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, {0, state.dataGlobal->NumOfTimeStepInHour * 24}, TotSurfaces);
            ort->loadConvectedNormal = 0.0;
            ort->loadConvectedWithPulse.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, {0, state.dataGlobal->NumOfTimeStepInHour * 24}, TotSurfaces);
            ort->loadConvectedWithPulse = 0.0;
            ort->netSurfRadSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, TotSurfaces);
            ort->netSurfRadSeq = 0.0;
            ort->decayCurveCool.allocate(state.dataGlobal->NumOfTimeStepInHour * 24, TotSurfaces);
            ort->decayCurveCool = 0.0;
            ort->decayCurveHeat.allocate(state.dataGlobal->NumOfTimeStepInHour * 24, TotSurfaces);
            ort->decayCurveHeat = 0.0;
            ort->ITABSFseq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, TotSurfaces);
            ort->ITABSFseq = 0.0;
            ort->TMULTseq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->TMULTseq = 0.0;
            ort->peopleInstantSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->peopleInstantSeq = 0.0;
            ort->peopleLatentSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->peopleLatentSeq = 0.0;
            ort->peopleRadSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->peopleRadSeq = 0.0;
            ort->lightInstantSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->lightInstantSeq = 0.0;
            ort->lightRetAirSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->lightRetAirSeq = 0.0;
            ort->lightLWRadSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->lightLWRadSeq = 0.0;
            ort->lightSWRadSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, TotSurfaces);
            ort->lightSWRadSeq = 0.0;
            ort->equipInstantSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->equipInstantSeq = 0.0;
            ort->equipLatentSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->equipLatentSeq = 0.0;
            ort->equipRadSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->equipRadSeq = 0.0;
            ort->refrigInstantSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->refrigInstantSeq = 0.0;
            ort->refrigRetAirSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->refrigRetAirSeq = 0.0;
            ort->refrigLatentSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->refrigLatentSeq = 0.0;
            ort->waterUseInstantSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->waterUseInstantSeq = 0.0;
            ort->waterUseLatentSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->waterUseLatentSeq = 0.0;
            ort->hvacLossInstantSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->hvacLossInstantSeq = 0.0;
            ort->hvacLossRadSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->hvacLossRadSeq = 0.0;
            ort->powerGenInstantSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->powerGenInstantSeq = 0.0;
            ort->powerGenRadSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->powerGenRadSeq = 0.0;
            ort->infilInstantSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->infilInstantSeq = 0.0;
            ort->infilLatentSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->infilLatentSeq = 0.0;
            ort->zoneVentInstantSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->zoneVentInstantSeq = 0.0;
            ort->zoneVentLatentSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->zoneVentLatentSeq = 0.0;
            ort->interZoneMixInstantSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->interZoneMixInstantSeq = 0.0;
            ort->interZoneMixLatentSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->interZoneMixLatentSeq = 0.0;
            ort->feneCondInstantSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            ort->feneCondInstantSeq = 0.0;
            ort->feneSolarRadSeq.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, TotSurfaces);
            ort->feneSolarRadSeq = 0.0;
            ort->AllocateLoadComponentArraysDoAllocate = false;
        }
    }

    void DeallocateLoadComponentArrays(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   August 2012
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Deallocate the arrays related to the load component report that will not
        //   be needed in the reporting.

        // METHODOLOGY EMPLOYED:
        //   Use the DEALLOCATE command

        // REFERENCES:
        // na

        // USE STATEMENTS:
        // na
        // Using/Aliasing
        using DataSurfaces::TotSurfaces;

        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        auto &ort(state.dataOutRptTab);
        ort->radiantPulseTimestep.deallocate();
        ort->radiantPulseReceived.deallocate();
        ort->loadConvectedWithPulse.deallocate();
    }

    void ComputeLoadComponentDecayCurve(EnergyPlusData &state)
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   August 2012
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // Determines the load component decay curve based on normal and pulse results from zone sizing.

        // METHODOLOGY EMPLOYED:
        // Decay curve is the fraction of the heat convected from a surface over the initial radiant heat
        // absorbed by the surface.

        // REFERENCES:
        // na

        // Using/Aliasing
        using DataSizing::CalcFinalZoneSizing;
        using DataSurfaces::Surface;
        using DataSurfaces::TotSurfaces;
        using DataZoneEquipment::ZoneEquipConfig;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS
        // na

        // DERIVED TYPE DEFINITIONS
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        static int ZoneNum(0);
        static int SurfNum(0);
        static int TimeStep(0);
        static int TimeOfPulse(0);
        static int CoolDesSelected(0); // design day selected for cooling
        static int HeatDesSelected(0); // design day selected for heating
        int i;
        Real64 diff;
        auto &ort(state.dataOutRptTab);

        for (SurfNum = 1; SurfNum <= TotSurfaces; ++SurfNum) {
            ZoneNum = Surface(SurfNum).Zone;
            if (ZoneNum == 0) continue;
            if (!ZoneEquipConfig(ZoneNum).IsControlled) continue;
            CoolDesSelected = CalcFinalZoneSizing(ZoneNum).CoolDDNum;
            // loop over timesteps after pulse occurred
            if (CoolDesSelected != 0) {
                TimeOfPulse = ort->radiantPulseTimestep(CoolDesSelected, ZoneNum);
                // if the CoolDesSelected time is on a different day than
                // when the pulse occurred, need to scan back and find when
                // the pulse occurred.
                if (TimeOfPulse == 0) {
                    for (i = CoolDesSelected; i >= 1; --i) {
                        TimeOfPulse = ort->radiantPulseTimestep(i, ZoneNum);
                        if (TimeOfPulse != 0) break;
                    }
                }
                if (TimeOfPulse == 0) TimeOfPulse = 1;
                for (TimeStep = TimeOfPulse; TimeStep <= state.dataGlobal->NumOfTimeStepInHour * 24; ++TimeStep) {
                    if (ort->radiantPulseReceived(CoolDesSelected, SurfNum) != 0.0) {
                        diff = ort->loadConvectedWithPulse(CoolDesSelected, TimeStep, SurfNum) - ort->loadConvectedNormal(CoolDesSelected, TimeStep, SurfNum);
                        ort->decayCurveCool(TimeStep - TimeOfPulse + 1, SurfNum) = -diff / ort->radiantPulseReceived(CoolDesSelected, SurfNum);
                    } else {
                        ort->decayCurveCool(TimeStep - TimeOfPulse + 1, SurfNum) = 0.0;
                    }
                }
            }
            HeatDesSelected = CalcFinalZoneSizing(ZoneNum).HeatDDNum;
            if (HeatDesSelected != 0) {
                TimeOfPulse = ort->radiantPulseTimestep(HeatDesSelected, ZoneNum);
                // scan back to the day that the heating pulse occurs, if necessary
                if (TimeOfPulse == 0) {
                    for (i = HeatDesSelected; i >= 1; --i) {
                        TimeOfPulse = ort->radiantPulseTimestep(i, ZoneNum);
                        if (TimeOfPulse != 0) break;
                    }
                }
                if (TimeOfPulse == 0) TimeOfPulse = 1;
                for (TimeStep = TimeOfPulse; TimeStep <= state.dataGlobal->NumOfTimeStepInHour * 24; ++TimeStep) {
                    if (ort->radiantPulseReceived(HeatDesSelected, SurfNum) != 0.0) {
                        diff = ort->loadConvectedWithPulse(HeatDesSelected, TimeStep, SurfNum) - ort->loadConvectedNormal(HeatDesSelected, TimeStep, SurfNum);
                        ort->decayCurveHeat(TimeStep - TimeOfPulse + 1, SurfNum) = -diff / ort->radiantPulseReceived(HeatDesSelected, SurfNum);
                    } else {
                        ort->decayCurveHeat(TimeStep - TimeOfPulse + 1, SurfNum) = 0.0;
                    }
                }
            }
        }

        if (state.dataGlobal->ShowDecayCurvesInEIO) {
            // show the line definition for the decay curves
            print(state.files.eio,
                  "! <Radiant to Convective Decay Curves for Cooling>,Zone Name, Surface Name, Time "
                  "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36\n");
            print(state.files.eio,
                  "! <Radiant to Convective Decay Curves for Heating>,Zone Name, Surface Name, Time "
                  "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36\n");
            // Put the decay curve into the EIO file
            for (int iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
                for (int kSurf : DataSurfaces::AllSurfaceListReportOrder) {
                    if (Surface(kSurf).Zone != iZone) continue;
                    print(state.files.eio, "{},{},{}", "Radiant to Convective Decay Curves for Cooling", Zone(iZone).Name, Surface(kSurf).Name);
                    for (int jTime = 1; jTime <= min(state.dataGlobal->NumOfTimeStepInHour * 24, 36); ++jTime) {
                        print(state.files.eio, ",{:6.3F}", ort->decayCurveCool(jTime, kSurf));
                    }
                    // put a line feed at the end of the line
                    print(state.files.eio, "\n");
                }

                for (int kSurf : DataSurfaces::AllSurfaceListReportOrder) {
                    if (Surface(kSurf).Zone != iZone) continue;
                    print(state.files.eio, "{},{},{}", "Radiant to Convective Decay Curves for Heating", Zone(iZone).Name, Surface(kSurf).Name);
                    for (int jTime = 1; jTime <= min(state.dataGlobal->NumOfTimeStepInHour * 24, 36); ++jTime) {
                        print(state.files.eio, ",{:6.3F}", ort->decayCurveHeat(jTime, kSurf));
                    }
                    // put a line feed at the end of the line
                    print(state.files.eio, "\n");
                }
            }
        }
    }

    void GatherComponentLoadsSurface(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   September 2012
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Gather values during sizing used for loads component report.

        // METHODOLOGY EMPLOYED:
        //   Save sequence of values for report during sizing.

        // REFERENCES:
        // na

        // USE STATEMENTS:
        // na
        // Using/Aliasing
        using DataSizing::CurOverallSimDay;
        using DataSurfaces::Surface;
        using DataSurfaces::TotSurfaces;
        using DataSurfaces::SurfWinGainConvGlazShadGapToZoneRep;
        using DataSurfaces::SurfWinGainConvGlazToZoneRep;
        using DataSurfaces::SurfWinGainConvShadeToZoneRep;
        using DataSurfaces::SurfWinGainFrameDividerToZoneRep;
        using DataZoneEquipment::ZoneEquipConfig;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        static int iSurf(0);
        static int ZoneNum(0);
        static int TimeStepInDay(0);
        static Array1D_int IntGainTypesTubular(1, {IntGainTypeOf_DaylightingDeviceTubular});
        auto &ort(state.dataOutRptTab);

        if (state.dataGlobal->CompLoadReportIsReq && !state.dataGlobal->isPulseZoneSizing) {
            TimeStepInDay = (state.dataGlobal->HourOfDay - 1) * state.dataGlobal->NumOfTimeStepInHour + state.dataGlobal->TimeStep;
            ort->feneCondInstantSeq(CurOverallSimDay, TimeStepInDay, _) = 0.0;
            for (iSurf = 1; iSurf <= TotSurfaces; ++iSurf) {
                ZoneNum = Surface(iSurf).Zone;
                if (ZoneNum == 0) continue;
                if (Surface(iSurf).Class != DataSurfaces::SurfaceClass::Window) continue;
                // IF (.not. ZoneEquipConfig(ZoneNum)%IsControlled) CYCLE
                ort->feneCondInstantSeq(CurOverallSimDay, TimeStepInDay, ZoneNum) +=
                    SurfWinGainConvGlazToZoneRep(iSurf) + SurfWinGainConvGlazShadGapToZoneRep(iSurf) + SurfWinGainConvShadeToZoneRep(iSurf) +
                            SurfWinGainFrameDividerToZoneRep(iSurf);
                // for now assume zero instant solar - may change related
                // to how blinds and shades absorb solar radiation and
                // convect that heat that timestep.
                // feneSolarInstantSeq(ZoneNum,TimeStepInDay,CurOverallSimDay) = 0
            }
            for (int izone = 1; izone <= state.dataGlobal->NumOfZones; ++izone) {
                Real64 tubularGain = 0.0;
                InternalHeatGains::SumInternalConvectionGainsByTypes(izone, IntGainTypesTubular, tubularGain);
                ort->feneCondInstantSeq(CurOverallSimDay, TimeStepInDay, izone) += tubularGain;
            }
        }
    }

    void GatherComponentLoadsHVAC(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   September 2012
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Gather values during sizing used for loads component report.

        // METHODOLOGY EMPLOYED:
        //   Save sequence of values for report during sizing.

        // REFERENCES:
        // na

        // USE STATEMENTS:
        // na
        // Using/Aliasing
        using DataHeatBalance::ZnAirRpt;
        using DataHVACGlobals::TimeStepSys;
        using DataSizing::CurOverallSimDay;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        static int iZone(0);
        static int TimeStepInDay(0);
        auto &ort(state.dataOutRptTab);

        if (state.dataGlobal->CompLoadReportIsReq && !state.dataGlobal->isPulseZoneSizing) {
            TimeStepInDay = (state.dataGlobal->HourOfDay - 1) * state.dataGlobal->NumOfTimeStepInHour + state.dataGlobal->TimeStep;
            for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
                ort->infilInstantSeq(CurOverallSimDay, TimeStepInDay, iZone) =
                    ((ZnAirRpt(iZone).InfilHeatGain - ZnAirRpt(iZone).InfilHeatLoss) / (TimeStepSys * DataGlobalConstants::SecInHour)); // zone infiltration
                if (AirflowNetwork::SimulateAirflowNetwork > AirflowNetwork::AirflowNetworkControlSimple) {
                    ort->infilInstantSeq(CurOverallSimDay, TimeStepInDay, iZone) +=
                        (AirflowNetwork::AirflowNetworkReportData(iZone).MultiZoneInfiSenGainW -
                         AirflowNetwork::AirflowNetworkReportData(iZone).MultiZoneInfiSenLossW); // air flow network
                }
                ort->infilLatentSeq(CurOverallSimDay, TimeStepInDay, iZone) =
                    ((ZnAirRpt(iZone).InfilLatentGain - ZnAirRpt(iZone).InfilLatentLoss) / (TimeStepSys * DataGlobalConstants::SecInHour)); // zone infiltration
                if (AirflowNetwork::SimulateAirflowNetwork > AirflowNetwork::AirflowNetworkControlSimple) {
                    ort->infilLatentSeq(CurOverallSimDay, TimeStepInDay, iZone) +=
                        (AirflowNetwork::AirflowNetworkReportData(iZone).MultiZoneInfiLatGainW -
                         AirflowNetwork::AirflowNetworkReportData(iZone).MultiZoneInfiLatLossW); // air flow network
                }

                ort->zoneVentInstantSeq(CurOverallSimDay, TimeStepInDay, iZone) =
                    ((ZnAirRpt(iZone).VentilHeatGain - ZnAirRpt(iZone).VentilHeatLoss) / (TimeStepSys * DataGlobalConstants::SecInHour)); // zone ventilation
                if (AirflowNetwork::SimulateAirflowNetwork > AirflowNetwork::AirflowNetworkControlSimple) {
                    ort->zoneVentInstantSeq(CurOverallSimDay, TimeStepInDay, iZone) +=
                        (AirflowNetwork::AirflowNetworkReportData(iZone).MultiZoneVentSenGainW -
                         AirflowNetwork::AirflowNetworkReportData(iZone).MultiZoneVentSenLossW); // air flow network
                }
                ort->zoneVentLatentSeq(CurOverallSimDay, TimeStepInDay, iZone) =
                    ((ZnAirRpt(iZone).VentilLatentGain - ZnAirRpt(iZone).VentilLatentLoss) / (TimeStepSys * DataGlobalConstants::SecInHour)); // zone ventilation
                if (AirflowNetwork::SimulateAirflowNetwork > AirflowNetwork::AirflowNetworkControlSimple) {
                    ort->zoneVentInstantSeq(CurOverallSimDay, TimeStepInDay, iZone) +=
                        (AirflowNetwork::AirflowNetworkReportData(iZone).MultiZoneVentLatGainW -
                         AirflowNetwork::AirflowNetworkReportData(iZone).MultiZoneVentLatLossW); // air flow network
                }

                ort->interZoneMixInstantSeq(CurOverallSimDay, TimeStepInDay, iZone) =
                    ((ZnAirRpt(iZone).MixHeatGain - ZnAirRpt(iZone).MixHeatLoss) / (TimeStepSys * DataGlobalConstants::SecInHour)); // zone mixing
                if (AirflowNetwork::SimulateAirflowNetwork > AirflowNetwork::AirflowNetworkControlSimple) {
                    ort->interZoneMixInstantSeq(CurOverallSimDay, TimeStepInDay, iZone) +=
                        (AirflowNetwork::AirflowNetworkReportData(iZone).MultiZoneMixSenGainW -
                         AirflowNetwork::AirflowNetworkReportData(iZone).MultiZoneMixSenLossW); // air flow network
                }
                ort->interZoneMixLatentSeq(CurOverallSimDay, TimeStepInDay, iZone) =
                    ((ZnAirRpt(iZone).MixLatentGain - ZnAirRpt(iZone).MixLatentLoss) / (TimeStepSys * DataGlobalConstants::SecInHour)); // zone mixing
                if (AirflowNetwork::SimulateAirflowNetwork > AirflowNetwork::AirflowNetworkControlSimple) {
                    ort->interZoneMixLatentSeq(CurOverallSimDay, TimeStepInDay, iZone) +=
                        (AirflowNetwork::AirflowNetworkReportData(iZone).MultiZoneMixLatGainW -
                         AirflowNetwork::AirflowNetworkReportData(iZone).MultiZoneMixLatLossW); // air flow network
                }
            }
        }
    }

    void WriteLoadComponentSummaryTables(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   March 2012
        //       MODIFIED       na
        //       RE-ENGINEERED  Amir Roth, Feb 2016 (unified with ComputeDelayedComponents to simplify code and debugging)

        // PURPOSE OF THIS SUBROUTINE:
        //   Write the tables for the ZoneLoadComponentSummary and
        //   ZoneLoadComponentDetail reports which summarize the major
        //   load components for each zone in the building.

        // METHODOLOGY EMPLOYED:
        //   Create arrays for the call to WriteTable and then call it.
        //   This report actually consists of many sub-tables each with
        //   its own call to WriteTable.
        // The overall methodology is explained below:
        //
        // Determine decay curve - Pulse of radiant heat which is about 5% of lighting and
        //   equipment input for a single timestep a few hours after
        //   cooling or heat is scheduled on for each zone [radiantPulseTimestep(iZone)].
        //   The radiant heat received on each wall is stored [radiantPulseReceived(jSurface)].
        //   The load convected in the normal case [loadConvectedNormal(jSurface, kTime, mode)]
        //   and in the case with the pulse [loadConvectedWithPulse(jSurface, kTime, mode)].
        //   The difference divided by the pulse received by each surface
        //   [radiantPulseReceived(jSurface)] is stored in [decayCurve(jSurface,kTime,mode)].
        //
        // Determine delayed loads - From the last timestep of the peak load on the zone
        //   working backwards any radiant heat that was absorbed by the wall from an internal gain
        //   or solar gain is multiplied by the appropriate timesteps in the decay curve
        //   [decayCurve(jSurface,kTime,mode)] for timesteps that make up
        //   the number of averaged timesteps are used to determine the peak load
        //   [NumTimeStepsInAvg]. The sum for all surfaces in the zone are added together to
        //   determine the delayed load.
        //
        // Determine instant loads - Average the convective portion of the internal gains
        //   for the timesteps made up of the peak load period. Average those across the peak
        //   load period.
        //
        // The comments from ComputeDelayedComponents which was incorporated into this routine follow:
        //
        // PURPOSE OF THIS SUBROUTINE:
        //   For load component report, convert the sequence of radiant gains
        //   for people and equipment and other internal loads into convective
        //   gains based on the decay curves.
        //
        // METHODOLOGY EMPLOYED:
        //   For each step of sequence from each design day, compute the
        //   contributions from previous timesteps multiplied by the decay
        //   curve. Rather than store every internal load's radiant contribution
        //   to each surface, the TMULT and ITABSF sequences were also stored
        //   which allocates the total radiant to each surface in the zone. The
        //   formula used is:
        //       QRadThermInAbs(SurfNum) = QL(NZ) * TMULT(NZ) * ITABSF(SurfNum)

        using DataHVACGlobals::NumPrimaryAirSys;
        using DataSizing::CalcFinalFacilitySizing;
        using DataSizing::CalcFinalZoneSizing;
        using DataSizing::SysSizPeakDDNum;
        using DataSurfaces::TotSurfaces;
        using DataZoneEquipment::ZoneEquipConfig;
        auto &ort(state.dataOutRptTab);

        if (!((ort->displayZoneComponentLoadSummary || ort->displayAirLoopComponentLoadSummary || ort->displayFacilityComponentLoadSummary) && state.dataGlobal->CompLoadReportIsReq))
            return;

        int coolDesSelected;
        int timeCoolMax;
        int heatDesSelected;
        int timeHeatMax;
        Real64 mult; // zone multiplier

        // Delayed components are moved into this function so that we can calculate them one zone at a time
        // with Array1D
        Array1D<Real64> peopleDelaySeqHeat;
        Array1D<Real64> peopleDelaySeqCool;
        Array1D<Real64> lightDelaySeqHeat;
        Array1D<Real64> lightDelaySeqCool;
        Array1D<Real64> equipDelaySeqHeat;
        Array1D<Real64> equipDelaySeqCool;
        Array1D<Real64> hvacLossDelaySeqHeat;
        Array1D<Real64> hvacLossDelaySeqCool;
        Array1D<Real64> powerGenDelaySeqHeat;
        Array1D<Real64> powerGenDelaySeqCool;
        Array1D<Real64> feneSolarDelaySeqHeat;
        Array1D<Real64> feneSolarDelaySeqCool;
        Array2D<Real64> surfDelaySeqHeat;
        Array2D<Real64> surfDelaySeqCool;

        peopleDelaySeqHeat.dimension(state.dataGlobal->NumOfTimeStepInHour * 24, 0.0);
        peopleDelaySeqHeat = 0.0;
        peopleDelaySeqCool.allocate(state.dataGlobal->NumOfTimeStepInHour * 24);
        peopleDelaySeqCool = 0.0;
        lightDelaySeqHeat.allocate(state.dataGlobal->NumOfTimeStepInHour * 24);
        lightDelaySeqHeat = 0.0;
        lightDelaySeqCool.allocate(state.dataGlobal->NumOfTimeStepInHour * 24);
        lightDelaySeqCool = 0.0;
        equipDelaySeqHeat.allocate(state.dataGlobal->NumOfTimeStepInHour * 24);
        equipDelaySeqHeat = 0.0;
        equipDelaySeqCool.allocate(state.dataGlobal->NumOfTimeStepInHour * 24);
        equipDelaySeqCool = 0.0;
        hvacLossDelaySeqHeat.allocate(state.dataGlobal->NumOfTimeStepInHour * 24);
        hvacLossDelaySeqHeat = 0.0;
        hvacLossDelaySeqCool.allocate(state.dataGlobal->NumOfTimeStepInHour * 24);
        hvacLossDelaySeqCool = 0.0;
        powerGenDelaySeqHeat.allocate(state.dataGlobal->NumOfTimeStepInHour * 24);
        powerGenDelaySeqHeat = 0.0;
        powerGenDelaySeqCool.allocate(state.dataGlobal->NumOfTimeStepInHour * 24);
        powerGenDelaySeqCool = 0.0;
        feneSolarDelaySeqHeat.allocate(state.dataGlobal->NumOfTimeStepInHour * 24);
        feneSolarDelaySeqHeat = 0.0;
        feneSolarDelaySeqCool.allocate(state.dataGlobal->NumOfTimeStepInHour * 24);
        feneSolarDelaySeqCool = 0.0;
        surfDelaySeqHeat.allocate(state.dataGlobal->NumOfTimeStepInHour * 24, TotSurfaces);
        surfDelaySeqHeat = 0.0;
        surfDelaySeqCool.allocate(state.dataGlobal->NumOfTimeStepInHour * 24, TotSurfaces);
        surfDelaySeqCool = 0.0;

        Array1D<CompLoadTablesType> ZoneHeatCompLoadTables; // for zone level component load summary output tables
        Array1D<CompLoadTablesType> ZoneCoolCompLoadTables;

        Array1D<CompLoadTablesType> AirLoopHeatCompLoadTables; // for airloop level component load summary output tables
        Array1D<CompLoadTablesType> AirLoopCoolCompLoadTables;
        Array1D<CompLoadTablesType> AirLoopZonesHeatCompLoadTables; // zone results used for airloop report - never directly output
        Array1D<CompLoadTablesType> AirLoopZonesCoolCompLoadTables;

        CompLoadTablesType FacilityHeatCompLoadTables; // for facility level component load summary output tables
        CompLoadTablesType FacilityCoolCompLoadTables;
        Array1D<CompLoadTablesType> FacilityZonesHeatCompLoadTables; // zone results used for facility report - never directly output
        Array1D<CompLoadTablesType> FacilityZonesCoolCompLoadTables;

        CompLoadTablesType curCompLoadTable; // active component load table

        // initialize arrays
        if (ort->displayZoneComponentLoadSummary) {
            ZoneHeatCompLoadTables.allocate(state.dataGlobal->NumOfZones);
            for (auto &e : ZoneHeatCompLoadTables) {
                e.cells.allocate(cPerArea, rGrdTot);
                e.cells = 0.;
                e.cellUsed.allocate(cPerArea, rGrdTot);
                e.cellUsed = false;
            }
            ZoneCoolCompLoadTables.allocate(state.dataGlobal->NumOfZones);
            for (auto &e : ZoneCoolCompLoadTables) {
                e.cells.allocate(cPerArea, rGrdTot);
                e.cells = 0.;
                e.cellUsed.allocate(cPerArea, rGrdTot);
                e.cellUsed = false;
            }
        }
        if (ort->displayAirLoopComponentLoadSummary) {
            AirLoopHeatCompLoadTables.allocate(NumPrimaryAirSys);
            for (auto &e : AirLoopHeatCompLoadTables) {
                e.cells.allocate(cPerArea, rGrdTot);
                e.cells = 0.;
                e.cellUsed.allocate(cPerArea, rGrdTot);
                e.cellUsed = false;
                e.zoneIndices.allocate(state.dataGlobal->NumOfZones); // only need to allocate this for the AirLoop
                e.zoneIndices = 0;
            }
            AirLoopCoolCompLoadTables.allocate(NumPrimaryAirSys);
            for (auto &e : AirLoopCoolCompLoadTables) {
                e.cells.allocate(cPerArea, rGrdTot);
                e.cells = 0.;
                e.cellUsed.allocate(cPerArea, rGrdTot);
                e.cellUsed = false;
                e.zoneIndices.allocate(state.dataGlobal->NumOfZones); // only need to allocate this for the AirLoop
                e.zoneIndices = 0;
            }
            AirLoopZonesHeatCompLoadTables.allocate(state.dataGlobal->NumOfZones);
            for (auto &e : AirLoopZonesHeatCompLoadTables) {
                e.cells.allocate(cPerArea, rGrdTot);
                e.cells = 0.;
                e.cellUsed.allocate(cPerArea, rGrdTot);
                e.cellUsed = false;
            }
            AirLoopZonesCoolCompLoadTables.allocate(state.dataGlobal->NumOfZones);
            for (auto &e : AirLoopZonesCoolCompLoadTables) {
                e.cells.allocate(cPerArea, rGrdTot);
                e.cells = 0.;
                e.cellUsed.allocate(cPerArea, rGrdTot);
                e.cellUsed = false;
            }
        }
        if (ort->displayFacilityComponentLoadSummary) {
            FacilityHeatCompLoadTables.cells.allocate(cPerArea, rGrdTot);
            FacilityHeatCompLoadTables.cells = 0.;
            FacilityHeatCompLoadTables.cellUsed.allocate(cPerArea, rGrdTot);
            FacilityHeatCompLoadTables.cellUsed = false;

            FacilityCoolCompLoadTables.cells.allocate(cPerArea, rGrdTot);
            FacilityCoolCompLoadTables.cells = 0.;
            FacilityCoolCompLoadTables.cellUsed.allocate(cPerArea, rGrdTot);
            FacilityCoolCompLoadTables.cellUsed = false;

            FacilityZonesHeatCompLoadTables.allocate(state.dataGlobal->NumOfZones);
            for (auto &e : FacilityZonesHeatCompLoadTables) {
                e.cells.allocate(cPerArea, rGrdTot);
                e.cells = 0.;
                e.cellUsed.allocate(cPerArea, rGrdTot);
                e.cellUsed = false;
            }
            FacilityZonesCoolCompLoadTables.allocate(state.dataGlobal->NumOfZones);
            for (auto &e : FacilityZonesCoolCompLoadTables) {
                e.cells.allocate(cPerArea, rGrdTot);
                e.cells = 0.;
                e.cellUsed.allocate(cPerArea, rGrdTot);
                e.cellUsed = false;
            }
        }

        // get the zone areas needed later
        Array1D<ZompComponentAreasType> ZoneComponentAreas;
        ZoneComponentAreas.allocate(state.dataGlobal->NumOfZones);
        GetZoneComponentAreas(state, ZoneComponentAreas);

        // ZoneComponentLoadSummary
        if (ort->displayZoneComponentLoadSummary) {
            for (int iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
                if (!ZoneEquipConfig(iZone).IsControlled) continue;
                if (allocated(CalcFinalZoneSizing)) {
                    coolDesSelected = CalcFinalZoneSizing(iZone).CoolDDNum;
                    ZoneCoolCompLoadTables(iZone).desDayNum = coolDesSelected;
                    timeCoolMax = CalcFinalZoneSizing(iZone).TimeStepNumAtCoolMax;
                    ZoneCoolCompLoadTables(iZone).timeStepMax = timeCoolMax;

                    GetDelaySequences(state,
                                      coolDesSelected,
                                      true,
                                      iZone,
                                      peopleDelaySeqCool,
                                      equipDelaySeqCool,
                                      hvacLossDelaySeqCool,
                                      powerGenDelaySeqCool,
                                      lightDelaySeqCool,
                                      feneSolarDelaySeqCool,
                                      ort->feneCondInstantSeq,
                                      surfDelaySeqCool);
                    ComputeTableBodyUsingMovingAvg(state,
                                                   ZoneCoolCompLoadTables(iZone).cells,
                                                   ZoneCoolCompLoadTables(iZone).cellUsed,
                                                   coolDesSelected,
                                                   timeCoolMax,
                                                   iZone,
                                                   peopleDelaySeqCool,
                                                   equipDelaySeqCool,
                                                   hvacLossDelaySeqCool,
                                                   powerGenDelaySeqCool,
                                                   lightDelaySeqCool,
                                                   feneSolarDelaySeqCool,
                                                   ort->feneCondInstantSeq,
                                                   surfDelaySeqCool);
                    CollectPeakZoneConditions(state, ZoneCoolCompLoadTables(iZone), coolDesSelected, timeCoolMax, iZone, true);
                    // send latent load info to coil summary report
                    coilSelectionReportObj->setZoneLatentLoadCoolingIdealPeak(iZone, ZoneCoolCompLoadTables(iZone).cells(cLatent, rGrdTot));

                    heatDesSelected = CalcFinalZoneSizing(iZone).HeatDDNum;
                    ZoneHeatCompLoadTables(iZone).desDayNum = heatDesSelected;
                    timeHeatMax = CalcFinalZoneSizing(iZone).TimeStepNumAtHeatMax;
                    ZoneHeatCompLoadTables(iZone).timeStepMax = timeHeatMax;

                    GetDelaySequences(state,
                                      heatDesSelected,
                                      false,
                                      iZone,
                                      peopleDelaySeqHeat,
                                      equipDelaySeqHeat,
                                      hvacLossDelaySeqHeat,
                                      powerGenDelaySeqHeat,
                                      lightDelaySeqHeat,
                                      feneSolarDelaySeqHeat,
                                      ort->feneCondInstantSeq,
                                      surfDelaySeqHeat);
                    ComputeTableBodyUsingMovingAvg(state,
                                                   ZoneHeatCompLoadTables(iZone).cells,
                                                   ZoneHeatCompLoadTables(iZone).cellUsed,
                                                   heatDesSelected,
                                                   timeHeatMax,
                                                   iZone,
                                                   peopleDelaySeqHeat,
                                                   equipDelaySeqHeat,
                                                   hvacLossDelaySeqHeat,
                                                   powerGenDelaySeqHeat,
                                                   lightDelaySeqHeat,
                                                   feneSolarDelaySeqHeat,
                                                   ort->feneCondInstantSeq,
                                                   surfDelaySeqHeat);
                    CollectPeakZoneConditions(state, ZoneHeatCompLoadTables(iZone), heatDesSelected, timeHeatMax, iZone, false);

                    // send latent load info to coil summary report
                    coilSelectionReportObj->setZoneLatentLoadHeatingIdealPeak(iZone, ZoneHeatCompLoadTables(iZone).cells(cLatent, rGrdTot));

                    AddAreaColumnForZone(iZone, ZoneComponentAreas, ZoneCoolCompLoadTables(iZone));
                    AddAreaColumnForZone(iZone, ZoneComponentAreas, ZoneHeatCompLoadTables(iZone));

                    AddTotalRowsForLoadSummary(ZoneCoolCompLoadTables(iZone));
                    AddTotalRowsForLoadSummary(ZoneHeatCompLoadTables(iZone));

                    ComputePeakDifference(ZoneCoolCompLoadTables(iZone));
                    ComputePeakDifference(ZoneHeatCompLoadTables(iZone));

                    // We delay the potential application of SI to IP conversion and actual output until after both the AirLoopComponentLoadSummary
                    // and FacilityComponentLoadSummary have been processed because below we try to retrieve the info directly when the timestamp
                    // would match (cf #7356), and if we converted right now, we would apply the conversion twice
                }
            }
        }

        // AirLoopComponentLoadSummary
        if (ort->displayAirLoopComponentLoadSummary && NumPrimaryAirSys > 0) {
            Array1D_int zoneToAirLoopCool;
            zoneToAirLoopCool.dimension(state.dataGlobal->NumOfZones);
            Array1D_int zoneToAirLoopHeat;
            zoneToAirLoopHeat.dimension(state.dataGlobal->NumOfZones);
            // set the peak day and time for each zone used by the airloops - use all zones connected to the airloop for both heating and cooling
            // (regardless of "heated" or "cooled" zone status)
            for (int iAirLoop = 1; iAirLoop <= NumPrimaryAirSys; ++iAirLoop) {
                zoneToAirLoopCool = 0;
                zoneToAirLoopHeat = 0;
                if (DataSizing::FinalSysSizing(iAirLoop).CoolingPeakLoadType == DataSizing::SensibleCoolingLoad) {
                    coolDesSelected = SysSizPeakDDNum(iAirLoop).SensCoolPeakDD;
                    if (coolDesSelected != 0) {
                        timeCoolMax = SysSizPeakDDNum(iAirLoop).TimeStepAtSensCoolPk(coolDesSelected);
                    } else {
                        timeCoolMax = 0;
                    }
                } else if (DataSizing::FinalSysSizing(iAirLoop).CoolingPeakLoadType == DataSizing::Ventilation) {
                    coolDesSelected = SysSizPeakDDNum(iAirLoop).CoolFlowPeakDD;
                    if (coolDesSelected != 0) {
                        timeCoolMax = SysSizPeakDDNum(iAirLoop).TimeStepAtCoolFlowPk(coolDesSelected);
                    } else {
                        timeCoolMax = 0;
                    }
                } else {
                    coolDesSelected = SysSizPeakDDNum(iAirLoop).TotCoolPeakDD;
                    if (coolDesSelected != 0) {
                        timeCoolMax = SysSizPeakDDNum(iAirLoop).TimeStepAtTotCoolPk(coolDesSelected);
                    } else {
                        timeCoolMax = 0;
                    }
                }
                heatDesSelected = SysSizPeakDDNum(iAirLoop).HeatPeakDD;
                if (heatDesSelected != 0) {
                    timeHeatMax = SysSizPeakDDNum(iAirLoop).TimeStepAtHeatPk(heatDesSelected);
                } else {
                    timeHeatMax = 0;
                }

                int NumZonesCooled = state.dataAirLoop->AirToZoneNodeInfo(iAirLoop).NumZonesCooled;
                for (int ZonesCooledNum = 1; ZonesCooledNum <= NumZonesCooled; ++ZonesCooledNum) { // loop over cooled zones
                    int CtrlZoneNum = state.dataAirLoop->AirToZoneNodeInfo(iAirLoop).CoolCtrlZoneNums(ZonesCooledNum);
                    zoneToAirLoopCool(CtrlZoneNum) = iAirLoop;
                    AirLoopZonesCoolCompLoadTables(CtrlZoneNum).desDayNum = coolDesSelected;
                    AirLoopZonesCoolCompLoadTables(CtrlZoneNum).timeStepMax = timeCoolMax;
                    zoneToAirLoopHeat(CtrlZoneNum) = iAirLoop;
                    AirLoopZonesHeatCompLoadTables(CtrlZoneNum).desDayNum = heatDesSelected;
                    AirLoopZonesHeatCompLoadTables(CtrlZoneNum).timeStepMax = timeHeatMax;
                }
                int NumZonesHeated = state.dataAirLoop->AirToZoneNodeInfo(iAirLoop).NumZonesHeated;
                for (int ZonesHeatedNum = 1; ZonesHeatedNum <= NumZonesHeated; ++ZonesHeatedNum) { // loop over heated zones
                    int CtrlZoneNum = state.dataAirLoop->AirToZoneNodeInfo(iAirLoop).HeatCtrlZoneNums(ZonesHeatedNum);
                    zoneToAirLoopCool(CtrlZoneNum) = iAirLoop;
                    AirLoopZonesCoolCompLoadTables(CtrlZoneNum).desDayNum = coolDesSelected;
                    AirLoopZonesCoolCompLoadTables(CtrlZoneNum).timeStepMax = timeCoolMax;
                    zoneToAirLoopHeat(CtrlZoneNum) = iAirLoop;
                    AirLoopZonesHeatCompLoadTables(CtrlZoneNum).desDayNum = heatDesSelected;
                    AirLoopZonesHeatCompLoadTables(CtrlZoneNum).timeStepMax = timeHeatMax;
                }

                // now go through the zones and if design day and time of max match the previously calculated zone results use those otherwise compute
                // them for specific design day and time of max
                for (int iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
                    if (!ZoneEquipConfig(iZone).IsControlled) continue;
                    // The ZoneCoolCompLoadTables already hasn't gotten a potential IP conversion yet, so we won't convert it twice.
                    if (ort->displayZoneComponentLoadSummary &&
                        (AirLoopZonesCoolCompLoadTables(iZone).desDayNum == ZoneCoolCompLoadTables(iZone).desDayNum) &&
                        (AirLoopZonesCoolCompLoadTables(iZone).timeStepMax == ZoneCoolCompLoadTables(iZone).timeStepMax)) {
                        AirLoopZonesCoolCompLoadTables(iZone) = ZoneCoolCompLoadTables(iZone);
                    } else {
                        coolDesSelected = AirLoopZonesCoolCompLoadTables(iZone).desDayNum;
                        timeCoolMax = AirLoopZonesCoolCompLoadTables(iZone).timeStepMax;

                        GetDelaySequences(state,
                                          coolDesSelected,
                                          true,
                                          iZone,
                                          peopleDelaySeqCool,
                                          equipDelaySeqCool,
                                          hvacLossDelaySeqCool,
                                          powerGenDelaySeqCool,
                                          lightDelaySeqCool,
                                          feneSolarDelaySeqCool,
                                          ort->feneCondInstantSeq,
                                          surfDelaySeqCool);
                        ComputeTableBodyUsingMovingAvg(state,
                                                       AirLoopZonesCoolCompLoadTables(iZone).cells,
                                                       AirLoopZonesCoolCompLoadTables(iZone).cellUsed,
                                                       coolDesSelected,
                                                       timeCoolMax,
                                                       iZone,
                                                       peopleDelaySeqCool,
                                                       equipDelaySeqCool,
                                                       hvacLossDelaySeqCool,
                                                       powerGenDelaySeqCool,
                                                       lightDelaySeqCool,
                                                       feneSolarDelaySeqCool,
                                                       ort->feneCondInstantSeq,
                                                       surfDelaySeqCool);
                        CollectPeakZoneConditions(state, AirLoopZonesCoolCompLoadTables(iZone), coolDesSelected, timeCoolMax, iZone, true);
                        AddAreaColumnForZone(iZone, ZoneComponentAreas, AirLoopZonesCoolCompLoadTables(iZone));
                    }
                    if (ort->displayZoneComponentLoadSummary &&
                        (AirLoopZonesHeatCompLoadTables(iZone).desDayNum == ZoneHeatCompLoadTables(iZone).desDayNum) &&
                        (AirLoopZonesHeatCompLoadTables(iZone).timeStepMax == ZoneHeatCompLoadTables(iZone).timeStepMax)) {
                        AirLoopZonesHeatCompLoadTables(iZone) = ZoneHeatCompLoadTables(iZone);
                    } else {
                        heatDesSelected = AirLoopZonesHeatCompLoadTables(iZone).desDayNum;
                        timeHeatMax = AirLoopZonesHeatCompLoadTables(iZone).timeStepMax;

                        GetDelaySequences(state,
                                          heatDesSelected,
                                          false,
                                          iZone,
                                          peopleDelaySeqHeat,
                                          equipDelaySeqHeat,
                                          hvacLossDelaySeqHeat,
                                          powerGenDelaySeqHeat,
                                          lightDelaySeqHeat,
                                          feneSolarDelaySeqHeat,
                                          ort->feneCondInstantSeq,
                                          surfDelaySeqHeat);
                        ComputeTableBodyUsingMovingAvg(state,
                                                       AirLoopZonesHeatCompLoadTables(iZone).cells,
                                                       AirLoopZonesHeatCompLoadTables(iZone).cellUsed,
                                                       heatDesSelected,
                                                       timeHeatMax,
                                                       iZone,
                                                       peopleDelaySeqHeat,
                                                       equipDelaySeqHeat,
                                                       hvacLossDelaySeqHeat,
                                                       powerGenDelaySeqHeat,
                                                       lightDelaySeqHeat,
                                                       feneSolarDelaySeqHeat,
                                                       ort->feneCondInstantSeq,
                                                       surfDelaySeqHeat);
                        CollectPeakZoneConditions(state, AirLoopZonesHeatCompLoadTables(iZone), heatDesSelected, timeHeatMax, iZone, false);
                        AddAreaColumnForZone(iZone, ZoneComponentAreas, AirLoopZonesHeatCompLoadTables(iZone));
                    }
                }
                // combine the zones for each air loop

                for (int iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
                    if (zoneToAirLoopCool(iZone) == iAirLoop) {
                        mult = Zone(iZone).Multiplier * Zone(iZone).ListMultiplier;
                        if (mult == 0.0) mult = 1.0;
                        CombineLoadCompResults(AirLoopCoolCompLoadTables(iAirLoop), AirLoopZonesCoolCompLoadTables(iZone), mult);
                    }
                    if (zoneToAirLoopHeat(iZone) == iAirLoop) {
                        mult = Zone(iZone).Multiplier * Zone(iZone).ListMultiplier;
                        if (mult == 0.0) mult = 1.0;
                        CombineLoadCompResults(AirLoopHeatCompLoadTables(iAirLoop), AirLoopZonesHeatCompLoadTables(iZone), mult);
                    }
                }

                ComputeEngineeringChecks(AirLoopCoolCompLoadTables(iAirLoop));
                ComputeEngineeringChecks(AirLoopHeatCompLoadTables(iAirLoop));

                AddTotalRowsForLoadSummary(AirLoopCoolCompLoadTables(iAirLoop));
                AddTotalRowsForLoadSummary(AirLoopHeatCompLoadTables(iAirLoop));

                ComputePeakDifference(AirLoopCoolCompLoadTables(iAirLoop));
                ComputePeakDifference(AirLoopHeatCompLoadTables(iAirLoop));

                CreateListOfZonesForAirLoop(state, AirLoopCoolCompLoadTables(iAirLoop), zoneToAirLoopCool, iAirLoop);
                CreateListOfZonesForAirLoop(state, AirLoopHeatCompLoadTables(iAirLoop), zoneToAirLoopHeat, iAirLoop);

                LoadSummaryUnitConversion(state, AirLoopCoolCompLoadTables(iAirLoop));
                LoadSummaryUnitConversion(state, AirLoopHeatCompLoadTables(iAirLoop));

                OutputCompLoadSummary(state, iOutputType::airLoopOutput, AirLoopCoolCompLoadTables(iAirLoop), AirLoopHeatCompLoadTables(iAirLoop), iAirLoop);
            }
        }

        // FacilityComponentLoadSummary
        if (ort->displayFacilityComponentLoadSummary) {

            coolDesSelected = CalcFinalFacilitySizing.CoolDDNum;
            timeCoolMax = CalcFinalFacilitySizing.TimeStepNumAtCoolMax;

            heatDesSelected = CalcFinalFacilitySizing.HeatDDNum;
            timeHeatMax = CalcFinalFacilitySizing.TimeStepNumAtHeatMax;

            for (int iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
                if (!ZoneEquipConfig(iZone).IsControlled) continue;
                mult = Zone(iZone).Multiplier * Zone(iZone).ListMultiplier;
                if (mult == 0.0) mult = 1.0;
                // The ZoneCoolCompLoadTables already hasn't gotten a potential IP conversion yet, so we won't convert it twice.
                if (ort->displayZoneComponentLoadSummary && (coolDesSelected == ZoneCoolCompLoadTables(iZone).desDayNum) &&
                    (timeCoolMax == ZoneCoolCompLoadTables(iZone).timeStepMax)) {
                    FacilityZonesCoolCompLoadTables(iZone) = ZoneCoolCompLoadTables(iZone);
                } else {
                    GetDelaySequences(state,
                                      coolDesSelected,
                                      true,
                                      iZone,
                                      peopleDelaySeqCool,
                                      equipDelaySeqCool,
                                      hvacLossDelaySeqCool,
                                      powerGenDelaySeqCool,
                                      lightDelaySeqCool,
                                      feneSolarDelaySeqCool,
                                      ort->feneCondInstantSeq,
                                      surfDelaySeqCool);
                    ComputeTableBodyUsingMovingAvg(state,
                                                   FacilityZonesCoolCompLoadTables(iZone).cells,
                                                   FacilityZonesCoolCompLoadTables(iZone).cellUsed,
                                                   coolDesSelected,
                                                   timeCoolMax,
                                                   iZone,
                                                   peopleDelaySeqCool,
                                                   equipDelaySeqCool,
                                                   hvacLossDelaySeqCool,
                                                   powerGenDelaySeqCool,
                                                   lightDelaySeqCool,
                                                   feneSolarDelaySeqCool,
                                                   ort->feneCondInstantSeq,
                                                   surfDelaySeqCool);
                    CollectPeakZoneConditions(state, FacilityZonesCoolCompLoadTables(iZone), coolDesSelected, timeCoolMax, iZone, true);
                    AddAreaColumnForZone(iZone, ZoneComponentAreas, FacilityZonesCoolCompLoadTables(iZone));
                }
                FacilityZonesCoolCompLoadTables(iZone).timeStepMax = timeCoolMax;
                FacilityZonesCoolCompLoadTables(iZone).desDayNum = coolDesSelected;
                CombineLoadCompResults(FacilityCoolCompLoadTables, FacilityZonesCoolCompLoadTables(iZone), mult);

                if (ort->displayZoneComponentLoadSummary && (heatDesSelected == ZoneHeatCompLoadTables(iZone).desDayNum) &&
                    (timeHeatMax == ZoneHeatCompLoadTables(iZone).timeStepMax)) {
                    FacilityZonesHeatCompLoadTables(iZone) = ZoneHeatCompLoadTables(iZone);
                } else {
                    GetDelaySequences(state,
                                      heatDesSelected,
                                      false,
                                      iZone,
                                      peopleDelaySeqHeat,
                                      equipDelaySeqHeat,
                                      hvacLossDelaySeqHeat,
                                      powerGenDelaySeqHeat,
                                      lightDelaySeqHeat,
                                      feneSolarDelaySeqHeat,
                                      ort->feneCondInstantSeq,
                                      surfDelaySeqHeat);
                    ComputeTableBodyUsingMovingAvg(state,
                                                   FacilityZonesHeatCompLoadTables(iZone).cells,
                                                   FacilityZonesHeatCompLoadTables(iZone).cellUsed,
                                                   heatDesSelected,
                                                   timeHeatMax,
                                                   iZone,
                                                   peopleDelaySeqHeat,
                                                   equipDelaySeqHeat,
                                                   hvacLossDelaySeqHeat,
                                                   powerGenDelaySeqHeat,
                                                   lightDelaySeqHeat,
                                                   feneSolarDelaySeqHeat,
                                                   ort->feneCondInstantSeq,
                                                   surfDelaySeqHeat);
                    CollectPeakZoneConditions(state, FacilityZonesHeatCompLoadTables(iZone), heatDesSelected, timeHeatMax, iZone, false);
                    AddAreaColumnForZone(iZone, ZoneComponentAreas, FacilityZonesHeatCompLoadTables(iZone));
                }
                FacilityZonesHeatCompLoadTables(iZone).timeStepMax = timeHeatMax;
                FacilityZonesHeatCompLoadTables(iZone).desDayNum = heatDesSelected;
                CombineLoadCompResults(FacilityHeatCompLoadTables, FacilityZonesHeatCompLoadTables(iZone), mult);
            }

            ComputeEngineeringChecks(FacilityCoolCompLoadTables);
            ComputeEngineeringChecks(FacilityHeatCompLoadTables);

            AddTotalRowsForLoadSummary(FacilityCoolCompLoadTables);
            AddTotalRowsForLoadSummary(FacilityHeatCompLoadTables);

            ComputePeakDifference(FacilityCoolCompLoadTables);
            ComputePeakDifference(FacilityHeatCompLoadTables);

            LoadSummaryUnitConversion(state, FacilityCoolCompLoadTables);
            LoadSummaryUnitConversion(state, FacilityHeatCompLoadTables);

            OutputCompLoadSummary(state, iOutputType::facilityOutput, FacilityCoolCompLoadTables, FacilityHeatCompLoadTables, 0);
        }

        // ZoneComponentLoadSummary: Now we convert and Display
        if (ort->displayZoneComponentLoadSummary) {
            for (int iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
                if (!ZoneEquipConfig(iZone).IsControlled) continue;
                if (allocated(CalcFinalZoneSizing)) {
                    LoadSummaryUnitConversion(state, ZoneCoolCompLoadTables(iZone));
                    LoadSummaryUnitConversion(state, ZoneHeatCompLoadTables(iZone));

                    OutputCompLoadSummary(state, iOutputType::zoneOutput, ZoneCoolCompLoadTables(iZone), ZoneHeatCompLoadTables(iZone), iZone);
                }
            }
        }

        ZoneHeatCompLoadTables.deallocate();
        ZoneCoolCompLoadTables.deallocate();
        AirLoopHeatCompLoadTables.deallocate();
        AirLoopCoolCompLoadTables.deallocate();
        AirLoopZonesHeatCompLoadTables.deallocate();
        AirLoopZonesCoolCompLoadTables.deallocate();
        FacilityZonesHeatCompLoadTables.deallocate();
        FacilityZonesCoolCompLoadTables.deallocate();

        peopleDelaySeqHeat.deallocate();
        peopleDelaySeqCool.deallocate();
        lightDelaySeqHeat.deallocate();
        lightDelaySeqCool.deallocate();
        equipDelaySeqHeat.deallocate();
        equipDelaySeqCool.deallocate();
        hvacLossDelaySeqHeat.deallocate();
        hvacLossDelaySeqCool.deallocate();
        powerGenDelaySeqHeat.deallocate();
        powerGenDelaySeqCool.deallocate();
        feneSolarDelaySeqHeat.deallocate();
        feneSolarDelaySeqCool.deallocate();
        surfDelaySeqHeat.deallocate();
        surfDelaySeqCool.deallocate();
    }

    // populate the delay sequence arrays for the component load summary table output
    void GetDelaySequences(EnergyPlusData &state,
                           int const &desDaySelected,
                           bool const &isCooling,
                           int const &zoneIndex,
                           Array1D<Real64> &peopleDelaySeq,
                           Array1D<Real64> &equipDelaySeq,
                           Array1D<Real64> &hvacLossDelaySeq,
                           Array1D<Real64> &powerGenDelaySeq,
                           Array1D<Real64> &lightDelaySeq,
                           Array1D<Real64> &feneSolarDelaySeq,
                           Array3D<Real64> &feneCondInstantSeq,
                           Array2D<Real64> &surfDelaySeq)
    {
        using DataHeatBalance::Zone;
        using DataSurfaces::Surface;

        // static bool initAdjFenDone(false); moved to anonymous namespace for unit testing
        static Array3D_bool adjFenDone;
        auto &ort(state.dataOutRptTab);

        if (!ort->initAdjFenDone) {
            adjFenDone.allocate(state.dataEnvrn->TotDesDays + state.dataEnvrn->TotRunDesPersDays, state.dataGlobal->NumOfTimeStepInHour * 24, state.dataGlobal->NumOfZones);
            adjFenDone = false;
            ort->initAdjFenDone = true;
        }

        int radEnclosureNum = Zone(zoneIndex).RadiantEnclosureNum;

        if (desDaySelected != 0) {

            Array2D<Real64> decayCurve;
            if (isCooling) {
                decayCurve = ort->decayCurveCool;
            } else {
                decayCurve = ort->decayCurveHeat;
            }

            for (int kTimeStep = 1; kTimeStep <= state.dataGlobal->NumOfTimeStepInHour * 24; ++kTimeStep) {
                Real64 peopleConvIntoZone = 0.0;
                Real64 equipConvIntoZone = 0.0;
                Real64 hvacLossConvIntoZone = 0.0;
                Real64 powerGenConvIntoZone = 0.0;
                Real64 lightLWConvIntoZone = 0.0;
                Real64 lightSWConvIntoZone = 0.0;
                Real64 feneSolarConvIntoZone = 0.0;
                Real64 adjFeneSurfNetRadSeq = 0.0;

                // code from ComputeDelayedComponents starts
                for (int jSurf = Zone(zoneIndex).SurfaceFirst; jSurf <= Zone(zoneIndex).SurfaceLast; ++jSurf) {
                    if (!Surface(jSurf).HeatTransSurf) continue; // Skip non-heat transfer surfaces

                    // for each time step, step back through time and apply decay curve to radiant heat for each end use absorbed in each surface
                    Real64 peopleConvFromSurf = 0.0;
                    Real64 equipConvFromSurf = 0.0;
                    Real64 hvacLossConvFromSurf = 0.0;
                    Real64 powerGenConvFromSurf = 0.0;
                    Real64 lightLWConvFromSurf = 0.0;
                    Real64 lightSWConvFromSurf = 0.0;
                    Real64 feneSolarConvFromSurf = 0.0;

                    for (int mStepBack = 1; mStepBack <= kTimeStep; ++mStepBack) {
                        int sourceStep = kTimeStep - mStepBack + 1;
                        Real64 thisQRadThermInAbsMult = ort->TMULTseq(desDaySelected, sourceStep, radEnclosureNum) *
                            ort->ITABSFseq(desDaySelected, sourceStep, jSurf) * Surface(jSurf).Area *
                            decayCurve(mStepBack, jSurf);
                        peopleConvFromSurf += ort->peopleRadSeq(desDaySelected, sourceStep, zoneIndex) * thisQRadThermInAbsMult;
                        equipConvFromSurf += ort->equipRadSeq(desDaySelected, sourceStep, zoneIndex) * thisQRadThermInAbsMult;
                        hvacLossConvFromSurf += ort->hvacLossRadSeq(desDaySelected, sourceStep, zoneIndex) * thisQRadThermInAbsMult;
                        powerGenConvFromSurf += ort->powerGenRadSeq(desDaySelected, sourceStep, zoneIndex) * thisQRadThermInAbsMult;
                        lightLWConvFromSurf += ort->lightLWRadSeq(desDaySelected, sourceStep, zoneIndex) * thisQRadThermInAbsMult;
                        // short wave is already accumulated by surface
                        lightSWConvFromSurf += ort->lightSWRadSeq(desDaySelected, sourceStep, jSurf) * decayCurve(mStepBack, jSurf);
                        feneSolarConvFromSurf += ort->feneSolarRadSeq(desDaySelected, sourceStep, jSurf) * decayCurve(mStepBack, jSurf);
                    } // for mStepBack

                    peopleConvIntoZone += peopleConvFromSurf;
                    equipConvIntoZone += equipConvFromSurf;
                    hvacLossConvIntoZone += hvacLossConvFromSurf;
                    powerGenConvIntoZone += powerGenConvFromSurf;
                    lightLWConvIntoZone += lightLWConvFromSurf;
                    lightSWConvIntoZone += lightSWConvFromSurf;
                    feneSolarConvIntoZone += feneSolarConvFromSurf;
                    // code from ComputeDelayedComponents ends
                    // determine the remaining convective heat from the surfaces that are not based
                    // on any of these other loads
                    // negative because heat from surface should be positive
                    surfDelaySeq(kTimeStep, jSurf) =
                        -ort->loadConvectedNormal(desDaySelected, kTimeStep, jSurf) - ort->netSurfRadSeq(desDaySelected, kTimeStep, jSurf) -
                        (peopleConvFromSurf + equipConvFromSurf + hvacLossConvFromSurf + powerGenConvFromSurf + lightLWConvFromSurf +
                         lightSWConvFromSurf +
                         feneSolarConvFromSurf); // remove net radiant for the surface
                                                 // also remove the net radiant component on the instanteous conduction for fenestration
                    if (Surface(jSurf).Class == DataSurfaces::SurfaceClass::Window) {
                        adjFeneSurfNetRadSeq += ort->netSurfRadSeq(desDaySelected, kTimeStep, jSurf);
                    }
                } // for jSurf
                peopleDelaySeq(kTimeStep) = peopleConvIntoZone;
                equipDelaySeq(kTimeStep) = equipConvIntoZone;
                hvacLossDelaySeq(kTimeStep) = hvacLossConvIntoZone;
                powerGenDelaySeq(kTimeStep) = powerGenConvIntoZone;
                // combine short wave (visible) and long wave (thermal) impacts
                lightDelaySeq(kTimeStep) = lightLWConvIntoZone + lightSWConvIntoZone;
                feneSolarDelaySeq(kTimeStep) = feneSolarConvIntoZone;
                // also remove the net radiant component on the instanteous conduction for fenestration
                if (!adjFenDone(desDaySelected, kTimeStep, zoneIndex)) {
                    feneCondInstantSeq(desDaySelected, kTimeStep, zoneIndex) -= adjFeneSurfNetRadSeq;
                    adjFenDone(desDaySelected, kTimeStep, zoneIndex) = true;
                }
            } // for kTimeStep

            decayCurve.deallocate();

        } // if desDaySelected != 0
    }

    // Used to construct the tabular output for a single cell in the component load summary reports based on moving average
    Real64 MovingAvgAtMaxTime(Array1S<Real64> const &dataSeq, int const &numTimeSteps, int const &maxTimeStep)
    {
        using DataSizing::NumTimeStepsInAvg;
        using General::MovingAvg;
        Array1D<Real64> AvgData; // sequence data after averaging
        AvgData.allocate(numTimeSteps);
        AvgData = 0.;
        MovingAvg(dataSeq * 1.0, numTimeSteps, NumTimeStepsInAvg, AvgData);
        return AvgData(maxTimeStep);
    }

    // set the load summary table cells based on the load sequences using moving averages to smooth out
    void ComputeTableBodyUsingMovingAvg(EnergyPlusData &state,
                                        Array2D<Real64> &resultCells,
                                        Array2D_bool &resCellsUsd,
                                        int const &desDaySelected,
                                        int const &timeOfMax,
                                        int const &zoneIndex,
                                        Array1D<Real64> const &peopleDelaySeq,
                                        Array1D<Real64> const &equipDelaySeq,
                                        Array1D<Real64> const &hvacLossDelaySeq,
                                        Array1D<Real64> const &powerGenDelaySeq,
                                        Array1D<Real64> const &lightDelaySeq,
                                        Array1D<Real64> const &feneSolarDelaySeq,
                                        Array3D<Real64> const &feneCondInstantSeq,
                                        Array2D<Real64> const &surfDelaySeq)
    {
        using DataHeatBalance::Zone;
        using DataSizing::CalcZoneSizing;
        using DataSizing::NumTimeStepsInAvg;
        using DataSurfaces::ExternalEnvironment;
        using DataSurfaces::Ground;
        using DataSurfaces::GroundFCfactorMethod;
        using DataSurfaces::KivaFoundation;
        using DataSurfaces::OSC;
        using DataSurfaces::OtherSideCoefCalcExt;
        using DataSurfaces::OtherSideCoefNoCalcExt;
        using DataSurfaces::OtherSideCondModeledExt;
        using DataSurfaces::Surface;
        using DataSurfaces::SurfaceClass;
        using DataSurfaces::TotSurfaces;
        using General::MovingAvg;

        Array1D<Real64> seqData;     // raw data sequence that has not been averaged yet
        Array1D<Real64> AvgData;     // sequence data after averaging
        Array1D<Real64> delayOpaque; // hold values for report for delayed opaque
        int curExtBoundCond;
        Real64 singleSurfDelay;
        auto &ort(state.dataOutRptTab);

        int NumOfTimeStepInDay = state.dataGlobal->NumOfTimeStepInHour * 24;

        resultCells = 0.;
        resCellsUsd = false;
        delayOpaque.allocate(rGrdTot);
        delayOpaque = 0.;
        AvgData.allocate(NumOfTimeStepInDay);
        AvgData = 0.;

        if (desDaySelected != 0 && timeOfMax != 0) {

            // PEOPLE
            resultCells(cSensInst, rPeople) = MovingAvgAtMaxTime(ort->peopleInstantSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensInst, rPeople) = true;
            resultCells(cLatent, rPeople) = MovingAvgAtMaxTime(ort->peopleLatentSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cLatent, rPeople) = true;
            resultCells(cSensDelay, rPeople) = MovingAvgAtMaxTime(peopleDelaySeq(_), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensDelay, rPeople) = true;

            // LIGHTS
            resultCells(cSensInst, rLights) = MovingAvgAtMaxTime(ort->lightInstantSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensInst, rLights) = true;
            resultCells(cSensRA, rLights) = MovingAvgAtMaxTime(ort->lightRetAirSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensRA, rLights) = true;
            resultCells(cSensDelay, rLights) = MovingAvgAtMaxTime(lightDelaySeq(_), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensDelay, rLights) = true;

            // EQUIPMENT
            resultCells(cSensInst, rEquip) = MovingAvgAtMaxTime(ort->equipInstantSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensInst, rEquip) = true;
            resultCells(cLatent, rEquip) = MovingAvgAtMaxTime(ort->equipLatentSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cLatent, rEquip) = true;
            resultCells(cSensDelay, rEquip) = MovingAvgAtMaxTime(equipDelaySeq(_), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensDelay, rEquip) = true;

            // REFRIGERATION EQUIPMENT
            resultCells(cSensInst, rRefrig) = MovingAvgAtMaxTime(ort->refrigInstantSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensInst, rRefrig) = true;
            resultCells(cSensRA, rRefrig) = MovingAvgAtMaxTime(ort->refrigRetAirSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensRA, rRefrig) = true;
            resultCells(cLatent, rRefrig) = MovingAvgAtMaxTime(ort->refrigLatentSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cLatent, rRefrig) = true;

            // WATER USE EQUIPMENT
            resultCells(cSensInst, rWaterUse) = MovingAvgAtMaxTime(ort->waterUseInstantSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensInst, rWaterUse) = true;
            resultCells(cLatent, rWaterUse) = MovingAvgAtMaxTime(ort->waterUseLatentSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cLatent, rWaterUse) = true;

            // HVAC EQUIPMENT LOSSES
            resultCells(cSensInst, rHvacLoss) = MovingAvgAtMaxTime(ort->hvacLossInstantSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensInst, rHvacLoss) = true;
            resultCells(cSensDelay, rHvacLoss) = MovingAvgAtMaxTime(hvacLossDelaySeq(_), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensDelay, rHvacLoss) = true;

            // POWER GENERATION EQUIPMENT
            resultCells(cSensInst, rPowerGen) = MovingAvgAtMaxTime(ort->powerGenInstantSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensInst, rPowerGen) = true;
            resultCells(cSensDelay, rPowerGen) = MovingAvgAtMaxTime(powerGenDelaySeq(_), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensDelay, rPowerGen) = true;

            // DOAS
            resultCells(cSensInst, rDOAS) = CalcZoneSizing(desDaySelected, zoneIndex).DOASHeatAddSeq(timeOfMax);
            resCellsUsd(cSensInst, rDOAS) = true;
            resultCells(cLatent, rDOAS) = CalcZoneSizing(desDaySelected, zoneIndex).DOASLatAddSeq(timeOfMax);
            resCellsUsd(cLatent, rDOAS) = true;

            // INFILTRATION
            resultCells(cSensInst, rInfil) = MovingAvgAtMaxTime(ort->infilInstantSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensInst, rInfil) = true;
            resultCells(cLatent, rInfil) = MovingAvgAtMaxTime(ort->infilLatentSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cLatent, rInfil) = true;

            // ZONE VENTILATION
            resultCells(cSensInst, rZoneVent) = MovingAvgAtMaxTime(ort->zoneVentInstantSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensInst, rZoneVent) = true;
            resultCells(cLatent, rZoneVent) = MovingAvgAtMaxTime(ort->zoneVentLatentSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cLatent, rZoneVent) = true;

            // INTERZONE MIXING
            resultCells(cSensInst, rIntZonMix) =
                MovingAvgAtMaxTime(ort->interZoneMixInstantSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensInst, rIntZonMix) = true;
            resultCells(cLatent, rIntZonMix) = MovingAvgAtMaxTime(ort->interZoneMixLatentSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cLatent, rIntZonMix) = true;

            // FENESTRATION CONDUCTION
            resultCells(cSensInst, rFeneCond) = MovingAvgAtMaxTime(feneCondInstantSeq(desDaySelected, _, zoneIndex), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensInst, rFeneCond) = true;

            // FENESTRATION SOLAR
            resultCells(cSensDelay, rFeneSolr) = MovingAvgAtMaxTime(feneSolarDelaySeq(_), NumOfTimeStepInDay, timeOfMax);
            resCellsUsd(cSensDelay, rFeneSolr) = true;

            // opaque surfaces - must combine individual surfaces by class and other side conditions
            delayOpaque = 0.0;
            for (int kSurf = Zone(zoneIndex).SurfaceFirst; kSurf <= Zone(zoneIndex).SurfaceLast; ++kSurf) {
                if (!Surface(kSurf).HeatTransSurf) continue; // Skip non-heat transfer surfaces

                curExtBoundCond = Surface(kSurf).ExtBoundCond;
                // if exterior is other side coefficients using ground preprocessor terms then
                // set it to ground instead of other side coefficients
                if (curExtBoundCond == OtherSideCoefNoCalcExt || curExtBoundCond == OtherSideCoefCalcExt) {
                    if (has_prefixi(OSC(Surface(kSurf).OSCPtr).Name, "surfPropOthSdCoef")) {
                        curExtBoundCond = Ground;
                    }
                }
                seqData = surfDelaySeq(_, kSurf);
                MovingAvg(seqData, NumOfTimeStepInDay, NumTimeStepsInAvg, AvgData);
                singleSurfDelay = AvgData(timeOfMax);
                {
                    auto const SELECT_CASE_var(Surface(kSurf).Class);
                    if (SELECT_CASE_var == SurfaceClass::Wall) {
                        {
                            auto const SELECT_CASE_var1(curExtBoundCond);
                            if (SELECT_CASE_var1 == ExternalEnvironment) {
                                delayOpaque(rExtWall) += singleSurfDelay;
                            } else if ((SELECT_CASE_var1 == Ground) || (SELECT_CASE_var1 == GroundFCfactorMethod) || (SELECT_CASE_var1 == KivaFoundation)) {
                                delayOpaque(rGrdWall) += singleSurfDelay;
                            } else if ((SELECT_CASE_var1 == OtherSideCoefNoCalcExt) || (SELECT_CASE_var1 == OtherSideCoefCalcExt) ||
                                       (SELECT_CASE_var1 == OtherSideCondModeledExt)) {
                                delayOpaque(rOtherWall) += singleSurfDelay;
                            } else { // interzone
                                delayOpaque(rIntZonWall) += singleSurfDelay;
                            }
                        }
                    } else if (SELECT_CASE_var == SurfaceClass::Floor) {
                        {
                            auto const SELECT_CASE_var1(curExtBoundCond);
                            if (SELECT_CASE_var1 == ExternalEnvironment) {
                                delayOpaque(rExtFlr) += singleSurfDelay;
                            } else if ((SELECT_CASE_var1 == Ground) || (SELECT_CASE_var1 == GroundFCfactorMethod) ||
                                       (SELECT_CASE_var1 == KivaFoundation)) {
                                delayOpaque(rGrdFlr) += singleSurfDelay;
                            } else if ((SELECT_CASE_var1 == OtherSideCoefNoCalcExt) || (SELECT_CASE_var1 == OtherSideCoefCalcExt) ||
                                       (SELECT_CASE_var1 == OtherSideCondModeledExt)) {
                                delayOpaque(rOtherFlr) += singleSurfDelay;
                            } else { // interzone
                                delayOpaque(rIntZonFlr) += singleSurfDelay;
                            }
                        }
                    } else if (SELECT_CASE_var == SurfaceClass::Roof) {
                        {
                            auto const SELECT_CASE_var1(curExtBoundCond);
                            if (SELECT_CASE_var1 == ExternalEnvironment) {
                                delayOpaque(rRoof) += singleSurfDelay;
                            } else if ((SELECT_CASE_var1 == Ground) || (SELECT_CASE_var1 == GroundFCfactorMethod) ||
                                       (SELECT_CASE_var1 == KivaFoundation) || (SELECT_CASE_var1 == OtherSideCoefNoCalcExt) ||
                                       (SELECT_CASE_var1 == OtherSideCoefCalcExt) || (SELECT_CASE_var1 == OtherSideCondModeledExt)) {
                                delayOpaque(rOtherRoof) += singleSurfDelay;
                            } else { // interzone
                                delayOpaque(rIntZonCeil) += singleSurfDelay;
                            }
                        }
                    } else if (SELECT_CASE_var == SurfaceClass::Door) {
                        delayOpaque(rOpqDoor) += singleSurfDelay;
                    }
                }
            }
            for (int k = rRoof; k <= rOtherFlr; ++k) {
                resultCells(cSensDelay, k) = delayOpaque(k);
                resCellsUsd(cSensDelay, k) = true;
            }
            resultCells(cSensDelay, rOpqDoor) = delayOpaque(rOpqDoor);
            resCellsUsd(cSensDelay, rOpqDoor) = true;
        }
    }

    // for the load summary report add values the peak conditions subtable
    void CollectPeakZoneConditions(EnergyPlusData &state,
        CompLoadTablesType &compLoad, int const &desDaySelected, int const &timeOfMax, int const &zoneIndex, bool const &isCooling)
    {
        using DataHeatBalance::People;
        using DataHeatBalance::TotPeople;
        using DataSizing::CalcFinalZoneSizing;
        using DataSizing::CoolPeakDateHrMin;
        using DataSizing::FinalZoneSizing;
        using DataSizing::HeatPeakDateHrMin;
        using DataSizing::SupplyAirTemperature;
        using Psychrometrics::PsyRhFnTdbWPb;
        using Psychrometrics::PsyTwbFnTdbWPb;

        if (timeOfMax != 0) {

            Real64 mult = Zone(zoneIndex).Multiplier * Zone(zoneIndex).ListMultiplier;
            if (mult == 0.0) mult = 1.0;

            if (isCooling) {
                // Time of Peak Load
                if ((size_t)desDaySelected <= state.dataWeatherManager->DesDayInput.size()) {
                    compLoad.peakDateHrMin = format("{}/{} {}",
                                                    state.dataWeatherManager->DesDayInput(desDaySelected).Month,
                                                    state.dataWeatherManager->DesDayInput(desDaySelected).DayOfMonth,
                                                    coilSelectionReportObj->getTimeText(state, timeOfMax));
                } else {
                    compLoad.peakDateHrMin = CoolPeakDateHrMin(zoneIndex);
                }

                // Outside Dry Bulb Temperature
                compLoad.outsideDryBulb = CalcFinalZoneSizing(zoneIndex).CoolOutTempSeq(timeOfMax);

                // Outside Wet Bulb Temperature
                // use standard sea level air pressure because air pressure is not tracked with sizing data
                if (CalcFinalZoneSizing(zoneIndex).CoolOutHumRatSeq(timeOfMax) < 1.0 &&
                    CalcFinalZoneSizing(zoneIndex).CoolOutHumRatSeq(timeOfMax) > 0.0) {
                    compLoad.outsideWetBulb = PsyTwbFnTdbWPb(state, CalcFinalZoneSizing(zoneIndex).CoolOutTempSeq(timeOfMax),
                                                             CalcFinalZoneSizing(zoneIndex).CoolOutHumRatSeq(timeOfMax),
                                                             101325.0);
                }

                // Outside Humidity Ratio at Peak
                compLoad.outsideHumRatio = CalcFinalZoneSizing(zoneIndex).CoolOutHumRatSeq(timeOfMax);

                // Zone Dry Bulb Temperature
                compLoad.zoneDryBulb = CalcFinalZoneSizing(zoneIndex).CoolZoneTempSeq(timeOfMax);

                // Zone Relative Humdity
                // use standard sea level air pressure because air pressure is not tracked with sizing data
                compLoad.zoneRelHum = PsyRhFnTdbWPb(state,
                    CalcFinalZoneSizing(zoneIndex).CoolZoneTempSeq(timeOfMax), CalcFinalZoneSizing(zoneIndex).CoolZoneHumRatSeq(timeOfMax), 101325.0);

                // Zone Humidity Ratio at Peak
                compLoad.zoneHumRatio = CalcFinalZoneSizing(zoneIndex).CoolZoneHumRatSeq(timeOfMax);

                // Peak Design Sensible Load
                compLoad.peakDesSensLoad = CalcFinalZoneSizing(zoneIndex).DesCoolLoad / mult; // change sign

                // Design Peak Load
                compLoad.designPeakLoad = FinalZoneSizing(zoneIndex).DesCoolLoad / mult;

                // Supply air temperature
                if (CalcFinalZoneSizing(zoneIndex).ZnCoolDgnSAMethod == SupplyAirTemperature) {
                    compLoad.supAirTemp = CalcFinalZoneSizing(zoneIndex).CoolDesTemp;
                } else {
                    Real64 DeltaTemp = -std::abs(CalcFinalZoneSizing(zoneIndex).CoolDesTempDiff);
                    compLoad.supAirTemp = DeltaTemp + CalcFinalZoneSizing(zoneIndex).ZoneTempAtCoolPeak;
                }

                // Main fan air flow
                compLoad.mainFanAirFlow = CalcFinalZoneSizing(zoneIndex).DesCoolVolFlow;

            } else {
                // Time of Peak Load
                if ((size_t)desDaySelected <= state.dataWeatherManager->DesDayInput.size()) {
                    compLoad.peakDateHrMin = format("{}/{} {}",
                                                    state.dataWeatherManager->DesDayInput(desDaySelected).Month,
                                                    state.dataWeatherManager->DesDayInput(desDaySelected).DayOfMonth,
                                                    coilSelectionReportObj->getTimeText(state, timeOfMax));
                } else {
                    compLoad.peakDateHrMin = HeatPeakDateHrMin(zoneIndex);
                }

                // Outside Dry Bulb Temperature
                compLoad.outsideDryBulb = CalcFinalZoneSizing(zoneIndex).HeatOutTempSeq(timeOfMax);

                // Outside Wet Bulb Temperature
                // use standard sea level air pressure because air pressure is not tracked with sizing data
                if (CalcFinalZoneSizing(zoneIndex).HeatOutHumRatSeq(timeOfMax) < 1.0 &&
                    CalcFinalZoneSizing(zoneIndex).HeatOutHumRatSeq(timeOfMax) > 0.0) {
                    compLoad.outsideWetBulb = PsyTwbFnTdbWPb(state, CalcFinalZoneSizing(zoneIndex).HeatOutTempSeq(timeOfMax),
                                                             CalcFinalZoneSizing(zoneIndex).HeatOutHumRatSeq(timeOfMax),
                                                             101325.0);
                }

                // Outside Humidity Ratio at Peak
                compLoad.outsideHumRatio = CalcFinalZoneSizing(zoneIndex).HeatOutHumRatSeq(timeOfMax);

                // Zone Dry Bulb Temperature
                compLoad.zoneDryBulb = CalcFinalZoneSizing(zoneIndex).HeatZoneTempSeq(timeOfMax);

                // Zone Relative Humdity
                // use standard sea level air pressure because air pressure is not tracked with sizing data
                compLoad.zoneRelHum = PsyRhFnTdbWPb(state,
                    CalcFinalZoneSizing(zoneIndex).HeatZoneTempSeq(timeOfMax), CalcFinalZoneSizing(zoneIndex).HeatZoneHumRatSeq(timeOfMax), 101325.0);

                // Zone Humidity Ratio at Peak
                compLoad.zoneHumRatio = CalcFinalZoneSizing(zoneIndex).HeatZoneHumRatSeq(timeOfMax);

                // Peak Design Sensible Load
                compLoad.peakDesSensLoad = -CalcFinalZoneSizing(zoneIndex).DesHeatLoad / mult; // change sign

                // Design Peak Load
                compLoad.designPeakLoad = -FinalZoneSizing(zoneIndex).DesHeatLoad / mult;

                // Supply air temperature
                if (CalcFinalZoneSizing(zoneIndex).ZnHeatDgnSAMethod == SupplyAirTemperature) {
                    compLoad.supAirTemp = CalcFinalZoneSizing(zoneIndex).HeatDesTemp;
                } else {
                    Real64 DeltaTemp = -std::abs(CalcFinalZoneSizing(zoneIndex).HeatDesTempDiff);
                    compLoad.supAirTemp = DeltaTemp + CalcFinalZoneSizing(zoneIndex).ZoneTempAtHeatPeak;
                }

                // Main fan air flow
                compLoad.mainFanAirFlow = CalcFinalZoneSizing(zoneIndex).DesHeatVolFlow;
            }

            // Outside air flow
            compLoad.outsideAirFlow = CalcFinalZoneSizing(zoneIndex).MinOA;

            // outside air %
            if (compLoad.mainFanAirFlow != 0.) {
                compLoad.outsideAirRatio = compLoad.outsideAirFlow / compLoad.mainFanAirFlow;
            }

            compLoad.floorArea = Zone(zoneIndex).FloorArea;

            if (compLoad.floorArea != 0.) {
                // airflow per floor area
                compLoad.airflowPerFlrArea = compLoad.mainFanAirFlow / compLoad.floorArea;

                // capacity per floor area
                compLoad.totCapPerArea = compLoad.designPeakLoad / compLoad.floorArea;
            }
            if (compLoad.designPeakLoad != 0.) {
                // airflow per capacity
                compLoad.airflowPerTotCap = compLoad.mainFanAirFlow / compLoad.designPeakLoad;

                // floor area per capacity
                compLoad.areaPerTotCap = Zone(zoneIndex).FloorArea / compLoad.designPeakLoad;
            }

            // Number of people
            Real64 totNumPeople = 0.;
            for (int iPeople = 1; iPeople <= TotPeople; ++iPeople) {
                if (zoneIndex == People(iPeople).ZonePtr) {
                    totNumPeople += People(iPeople).NumberOfPeople;
                }
            }
            compLoad.numPeople = totNumPeople;
        }
    }

    void ComputeEngineeringChecks(CompLoadTablesType &compLoad)
    {
        // outside air %
        if (compLoad.mainFanAirFlow != 0.) {
            compLoad.outsideAirRatio = compLoad.outsideAirFlow / compLoad.mainFanAirFlow;
        }

        if (compLoad.floorArea != 0.) {
            // airflow per floor area
            compLoad.airflowPerFlrArea = compLoad.mainFanAirFlow / compLoad.floorArea;

            // capacity per floor area
            compLoad.totCapPerArea = compLoad.designPeakLoad / compLoad.floorArea;
        }
        if (compLoad.designPeakLoad != 0.) {
            // airflow per capacity
            compLoad.airflowPerTotCap = compLoad.mainFanAirFlow / compLoad.designPeakLoad;

            // floor area per capacity
            compLoad.areaPerTotCap = compLoad.floorArea / compLoad.designPeakLoad;
        }
    }

    // gather the areas used in the load component tables
    void GetZoneComponentAreas(EnergyPlusData &state, Array1D<ZompComponentAreasType> &areas)
    {
        using namespace DataSurfaces;

        for (int iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
            areas(iZone).floor = Zone(iZone).FloorArea;
        }

        for (auto curSurface : Surface) {
            if (!curSurface.HeatTransSurf) continue;
            bool isExterior = curSurface.ExtBoundCond == ExternalEnvironment || curSurface.ExtBoundCond == OtherSideCondModeledExt;
            bool isTouchingGround =
                curSurface.ExtBoundCond == Ground || curSurface.ExtBoundCond == GroundFCfactorMethod || curSurface.ExtBoundCond == KivaFoundation;
            int curZoneIndex = curSurface.Zone;
            // ZoneData curZone = Zone(curSurface.Zone);
            if (curSurface.Class == SurfaceClass::Wall) {
                if (isExterior) {
                    areas(curZoneIndex).extWall += curSurface.GrossArea;
                } else if (isTouchingGround) {
                    areas(curZoneIndex).grndCntWall += curSurface.GrossArea;
                } else {
                    areas(curZoneIndex).intZoneWall += curSurface.GrossArea;
                }
            } else if (curSurface.Class == SurfaceClass::Roof) {
                if (isExterior) {
                    areas(curZoneIndex).roof += curSurface.GrossArea;
                } else {
                    areas(curZoneIndex).ceiling += curSurface.GrossArea;
                }
            } else if (curSurface.Class == SurfaceClass::Floor) {
                if (isExterior) {
                    areas(curZoneIndex).extFloor += curSurface.GrossArea;
                } else if (isTouchingGround) {
                    areas(curZoneIndex).grndCntFloor += curSurface.GrossArea;
                } else {
                    areas(curZoneIndex).intZoneFloor += curSurface.GrossArea;
                }
            } else if (curSurface.Class == SurfaceClass::Window || curSurface.Class == SurfaceClass::TDD_Dome) {
                areas(curZoneIndex).fenestration += curSurface.GrossArea;
            } else if (curSurface.Class == SurfaceClass::Door || curSurface.Class == SurfaceClass::GlassDoor) {
                areas(curZoneIndex).door += curSurface.GrossArea;
            }
        }
    }

    // adds the area column for the load component tables
    void AddAreaColumnForZone(int const &zoneNum, Array1D<ZompComponentAreasType> const &compAreas, CompLoadTablesType &compLoad)
    {
        compLoad.cells(cArea, rPeople) = compAreas(zoneNum).floor;
        compLoad.cellUsed(cArea, rPeople) = true;

        compLoad.cells(cArea, rLights) = compAreas(zoneNum).floor;
        compLoad.cellUsed(cArea, rLights) = true;

        compLoad.cells(cArea, rEquip) = compAreas(zoneNum).floor;
        compLoad.cellUsed(cArea, rEquip) = true;

        compLoad.cells(cArea, rRefrig) = compAreas(zoneNum).floor;
        compLoad.cellUsed(cArea, rRefrig) = true;

        compLoad.cells(cArea, rWaterUse) = compAreas(zoneNum).floor;
        compLoad.cellUsed(cArea, rWaterUse) = true;

        compLoad.cells(cArea, rInfil) = compAreas(zoneNum).extWall;
        compLoad.cellUsed(cArea, rInfil) = true;

        compLoad.cells(cArea, rRoof) = compAreas(zoneNum).roof;
        compLoad.cellUsed(cArea, rRoof) = true;

        compLoad.cells(cArea, rIntZonCeil) = compAreas(zoneNum).ceiling;
        compLoad.cellUsed(cArea, rIntZonCeil) = true;

        compLoad.cells(cArea, rOtherRoof) = compAreas(zoneNum).roof;
        compLoad.cellUsed(cArea, rOtherRoof) = true;

        compLoad.cells(cArea, rExtWall) = compAreas(zoneNum).extWall;
        compLoad.cellUsed(cArea, rExtWall) = true;

        compLoad.cells(cArea, rIntZonWall) = compAreas(zoneNum).intZoneWall;
        compLoad.cellUsed(cArea, rIntZonWall) = true;

        compLoad.cells(cArea, rGrdWall) = compAreas(zoneNum).grndCntWall;
        compLoad.cellUsed(cArea, rGrdWall) = true;

        compLoad.cells(cArea, rOtherWall) = compAreas(zoneNum).extWall;
        compLoad.cellUsed(cArea, rOtherWall) = true;

        compLoad.cells(cArea, rExtFlr) = compAreas(zoneNum).extFloor;
        compLoad.cellUsed(cArea, rExtFlr) = true;

        compLoad.cells(cArea, rIntZonFlr) = compAreas(zoneNum).intZoneFloor;
        compLoad.cellUsed(cArea, rIntZonFlr) = true;

        compLoad.cells(cArea, rGrdFlr) = compAreas(zoneNum).grndCntFloor;
        compLoad.cellUsed(cArea, rGrdFlr) = true;

        compLoad.cells(cArea, rOtherFlr) = compAreas(zoneNum).intZoneFloor;
        compLoad.cellUsed(cArea, rOtherFlr) = true;

        compLoad.cells(cArea, rFeneCond) = compAreas(zoneNum).fenestration;
        compLoad.cellUsed(cArea, rFeneCond) = true;

        compLoad.cells(cArea, rFeneSolr) = compAreas(zoneNum).fenestration;
        compLoad.cellUsed(cArea, rFeneSolr) = true;

        compLoad.cells(cArea, rOpqDoor) = compAreas(zoneNum).door;
        compLoad.cellUsed(cArea, rOpqDoor) = true;
    }

    // Used for the AirLoop and Facility level load component tables to sum the results from invidual zones
    void CombineLoadCompResults(CompLoadTablesType &compLoadTotal, CompLoadTablesType const &compLoadPartial, Real64 const &multiplier)
    {
        // sum the main results
        for (int col = 1; col <= cPerArea; ++col) {
            for (int row = 1; row <= rGrdTot; ++row) {
                compLoadTotal.cells(col, row) += compLoadPartial.cells(col, row) * multiplier;
                compLoadTotal.cellUsed(col, row) = compLoadTotal.cellUsed(col, row) || compLoadPartial.cellUsed(col, row);
            }
        }

        // take the partial value for these
        compLoadTotal.desDayNum = compLoadPartial.desDayNum;
        compLoadTotal.timeStepMax = compLoadPartial.timeStepMax;
        compLoadTotal.peakDateHrMin = compLoadPartial.peakDateHrMin;
        compLoadTotal.outsideDryBulb = compLoadPartial.outsideDryBulb;
        compLoadTotal.outsideWetBulb = compLoadPartial.outsideWetBulb;
        compLoadTotal.outsideHumRatio = compLoadPartial.outsideHumRatio;
        compLoadTotal.zoneDryBulb = compLoadPartial.zoneDryBulb;
        compLoadTotal.zoneRelHum = compLoadPartial.zoneRelHum;
        compLoadTotal.zoneHumRatio = compLoadPartial.zoneHumRatio;
        compLoadTotal.supAirTemp = compLoadPartial.supAirTemp;

        // sum the peak related values
        compLoadTotal.designPeakLoad += compLoadPartial.designPeakLoad * multiplier;
        compLoadTotal.diffDesignPeak += compLoadPartial.diffDesignPeak * multiplier;
        compLoadTotal.peakDesSensLoad += compLoadPartial.peakDesSensLoad * multiplier;
        compLoadTotal.estInstDelSensLoad += compLoadPartial.estInstDelSensLoad * multiplier;
        compLoadTotal.diffPeakEst += compLoadPartial.diffPeakEst * multiplier;
        compLoadTotal.mainFanAirFlow += compLoadPartial.mainFanAirFlow * multiplier;
        compLoadTotal.outsideAirFlow += compLoadPartial.outsideAirFlow * multiplier;

        // sum the engineering checks
        compLoadTotal.numPeople += compLoadPartial.numPeople * multiplier;
        compLoadTotal.floorArea += compLoadPartial.floorArea * multiplier;
    }

    // create the total row and total columns for the load summary tables
    void AddTotalRowsForLoadSummary(CompLoadTablesType &compLoadTotal)
    {

        // zero the grand total -total cell
        compLoadTotal.cells(cTotal, rGrdTot) = 0.;
        compLoadTotal.cellUsed(cTotal, rGrdTot) = true;

        // zero the grand total row
        for (int col = 1; col <= cLatent; ++col) {
            compLoadTotal.cells(col, rGrdTot) = 0.;
            compLoadTotal.cellUsed(col, rGrdTot) = true;
        }

        for (int row = 1; row <= rOpqDoor; ++row) {
            // zero the total column
            compLoadTotal.cells(cTotal, row) = 0.;
            compLoadTotal.cellUsed(cTotal, row) = true;
            for (int col = 1; col <= cLatent; ++col) {
                // add the cell to the grand total row and total column
                if (compLoadTotal.cellUsed(col, row)) {
                    compLoadTotal.cells(cTotal, row) += compLoadTotal.cells(col, row);
                    compLoadTotal.cells(col, rGrdTot) += compLoadTotal.cells(col, row);
                    compLoadTotal.cells(cTotal, rGrdTot) += compLoadTotal.cells(col, row);
                }
            }
        }

        // compute the % grand total column
        Real64 grandTotalTotal = compLoadTotal.cells(cTotal, rGrdTot);
        if (grandTotalTotal != 0.0) {
            for (int row = 1; row <= rOpqDoor; ++row) {
                compLoadTotal.cells(cPerc, row) = 100 * compLoadTotal.cells(cTotal, row) / grandTotalTotal;
                compLoadTotal.cellUsed(cPerc, row) = true;
            }
        }
        // compute the Total per Area column
        for (int row = 1; row <= rOpqDoor; ++row) {
            if (compLoadTotal.cellUsed(cTotal, row) && compLoadTotal.cells(cArea, row) != 0.) {
                compLoadTotal.cells(cPerArea, row) = compLoadTotal.cells(cTotal, row) / compLoadTotal.cells(cArea, row);
                compLoadTotal.cellUsed(cPerArea, row) = true;
            }
        }
    }

    // compute the peak difference between actual and estimated load in load component summary peak conditions table
    void ComputePeakDifference(CompLoadTablesType &compLoad)
    {
        // Estimated Instant + Delayed Sensible Load
        compLoad.estInstDelSensLoad = compLoad.cells(cSensInst, rGrdTot) + compLoad.cells(cSensDelay, rGrdTot);

        // Difference
        compLoad.diffPeakEst = compLoad.peakDesSensLoad - compLoad.estInstDelSensLoad;

        // Peak Design Diff
        compLoad.diffDesignPeak = compLoad.designPeakLoad - compLoad.peakDesSensLoad;
    }

    // apply unit conversions to the load components summary tables
    void LoadSummaryUnitConversion(EnergyPlusData &state, CompLoadTablesType &compLoadTotal)
    {
        auto &ort(state.dataOutRptTab);

        if (ort->unitsStyle == iUnitsStyle::InchPound) {
            Real64 powerConversion = getSpecificUnitMultiplier(state, "W", "Btu/h");
            Real64 areaConversion = getSpecificUnitMultiplier(state, "m2", "ft2");
            Real64 powerPerAreaConversion = getSpecificUnitMultiplier(state, "W/m2", "Btu/h-ft2");
            Real64 airFlowConversion = getSpecificUnitMultiplier(state, "m3/s", "ft3/min");
            Real64 airFlowPerAreaConversion = getSpecificUnitMultiplier(state, "m3/s-m2", "ft3/min-ft2");
            Real64 powerPerFlowLiquidConversion = getSpecificUnitMultiplier(state, "W-s/m3", "W-min/gal");
            for (int row = 1; row <= rGrdTot; ++row) {
                for (int col = 1; col <= cTotal; ++col) {
                    if (compLoadTotal.cellUsed(col, row)) {
                        compLoadTotal.cells(col, row) *= powerConversion;
                    }
                }
                if (compLoadTotal.cellUsed(cPerArea, row)) {
                    compLoadTotal.cells(cPerArea, row) *= powerConversion;
                }
                if (compLoadTotal.cellUsed(cArea, row)) {
                    compLoadTotal.cells(cArea, row) *= areaConversion;
                }
                if (compLoadTotal.cellUsed(cPerArea, row)) {
                    compLoadTotal.cells(cPerArea, row) *= powerPerAreaConversion;
                }
            }
            int tempConvIndx = getSpecificUnitIndex(state, "C", "F");
            compLoadTotal.outsideDryBulb = ConvertIP(state, tempConvIndx, compLoadTotal.outsideDryBulb);
            compLoadTotal.outsideWetBulb = ConvertIP(state, tempConvIndx, compLoadTotal.outsideWetBulb);
            compLoadTotal.zoneDryBulb = ConvertIP(state, tempConvIndx, compLoadTotal.zoneDryBulb);
            compLoadTotal.peakDesSensLoad *= powerConversion;

            compLoadTotal.supAirTemp = ConvertIP(state, tempConvIndx, compLoadTotal.supAirTemp);
            compLoadTotal.mixAirTemp = ConvertIP(state, tempConvIndx, compLoadTotal.mixAirTemp);
            compLoadTotal.mainFanAirFlow *= airFlowConversion;
            compLoadTotal.outsideAirFlow *= airFlowConversion;
            compLoadTotal.designPeakLoad *= powerConversion;
            compLoadTotal.diffDesignPeak *= powerConversion;

            compLoadTotal.estInstDelSensLoad *= powerConversion;
            compLoadTotal.diffPeakEst *= powerConversion;

            compLoadTotal.airflowPerFlrArea *= airFlowPerAreaConversion;
            if (powerConversion != 0.) {
                compLoadTotal.airflowPerTotCap = compLoadTotal.airflowPerTotCap * airFlowPerAreaConversion / powerConversion;
                compLoadTotal.areaPerTotCap = compLoadTotal.areaPerTotCap * areaConversion / powerConversion;
            }
            if (areaConversion != 0.) {
                compLoadTotal.totCapPerArea = compLoadTotal.totCapPerArea * powerConversion / areaConversion;
            }
            compLoadTotal.chlPumpPerFlow *= powerPerFlowLiquidConversion;
            compLoadTotal.cndPumpPerFlow *= powerPerFlowLiquidConversion;
        }
    }

    // make a list of the zones for the airloop component loads report
    void CreateListOfZonesForAirLoop(EnergyPlusData &state, CompLoadTablesType &compLoad, Array1D_int const &zoneToAirLoop, int const &curAirLoop)
    {
        int counter = 0;
        for (int zi = 1; zi <= state.dataGlobal->NumOfZones; ++zi) {
            if (zoneToAirLoop(zi) == curAirLoop) {
                ++counter;
                compLoad.zoneIndices(counter) = zi;
            }
        }
    }

    // provide output from the load component summary tables
    void OutputCompLoadSummary(EnergyPlusData &state,
                               iOutputType const &kind,
                               CompLoadTablesType const &compLoadCool,
                               CompLoadTablesType const &compLoadHeat,
                               int const &zoneOrAirLoopIndex)
    {
        CompLoadTablesType curCompLoad;
        bool writeOutput;
        Array1D_string columnHead;
        Array1D_int columnWidth;
        Array1D_string rowHead;
        Array2D_string tableBody; //(row, column)

        std::string reportName;
        std::string zoneAirLoopFacilityName;

        auto &ort(state.dataOutRptTab);

        if (kind == iOutputType::zoneOutput && ort->displayZoneComponentLoadSummary) {
            reportName = "Zone Component Load Summary";
            zoneAirLoopFacilityName = Zone(zoneOrAirLoopIndex).Name;
            writeOutput = true;
        } else if (kind == iOutputType::airLoopOutput && ort->displayAirLoopComponentLoadSummary) {
            reportName = "AirLoop Component Load Summary";
            zoneAirLoopFacilityName = DataSizing::FinalSysSizing(zoneOrAirLoopIndex).AirPriLoopName;
            writeOutput = true;
        } else if (kind == iOutputType::facilityOutput && ort->displayFacilityComponentLoadSummary) {
            reportName = "Facility Component Load Summary";
            zoneAirLoopFacilityName = "Facility";
            writeOutput = true;
        } else {
            writeOutput = false;
        }
        if (writeOutput) {
            WriteReportHeaders(state, reportName, zoneAirLoopFacilityName, OutputProcessor::StoreType::Averaged);
            std::string peakLoadCompName;
            std::string peakCondName;
            std::string zonesIncludedName;
            std::string engineeringCheckName;
            for (int coolHeat = 1; coolHeat <= 2; ++coolHeat) {
                tableBody.allocate(cPerArea, rGrdTot);
                tableBody = "";
                if (coolHeat == 1) {
                    curCompLoad = compLoadCool;
                    peakLoadCompName = "Estimated Cooling Peak Load Components";
                    peakCondName = "Cooling Peak Conditions";
                    zonesIncludedName = "Zones Included for Cooling";
                    engineeringCheckName = "Engineering Checks for Cooling";
                } else {
                    curCompLoad = compLoadHeat;
                    peakLoadCompName = "Estimated Heating Peak Load Components";
                    peakCondName = "Heating Peak Conditions";
                    zonesIncludedName = "Zones Included for Heating";
                    engineeringCheckName = "Engineering Checks for Heating";
                }
                // move number array into string array
                for (int c = 1; c <= cPerArea; ++c) {
                    for (int r = 1; r <= rGrdTot; ++r) { // to last row before total
                        if (curCompLoad.cellUsed(c, r)) {
                            tableBody(c, r) = RealToStr(curCompLoad.cells(c, r), 2);
                        }
                    }
                }
                rowHead.allocate(rGrdTot);
                // internal gains
                rowHead(rPeople) = "People";
                rowHead(rLights) = "Lights";
                rowHead(rEquip) = "Equipment";
                rowHead(rRefrig) = "Refrigeration Equipment";
                rowHead(rWaterUse) = "Water Use Equipment";
                rowHead(rPowerGen) = "Power Generation Equipment";
                rowHead(rHvacLoss) = "HVAC Equipment Losses";
                rowHead(rRefrig) = "Refrigeration";
                // misc
                rowHead(rDOAS) = "DOAS Direct to Zone";
                rowHead(rInfil) = "Infiltration";
                rowHead(rZoneVent) = "Zone Ventilation";
                rowHead(rIntZonMix) = "Interzone Mixing";
                // opaque surfaces
                rowHead(rRoof) = "Roof";
                rowHead(rIntZonCeil) = "Interzone Ceiling";
                rowHead(rOtherRoof) = "Other Roof";
                rowHead(rExtWall) = "Exterior Wall";
                rowHead(rIntZonWall) = "Interzone Wall";
                rowHead(rGrdWall) = "Ground Contact Wall";
                rowHead(rOtherWall) = "Other Wall";
                rowHead(rExtFlr) = "Exterior Floor";
                rowHead(rIntZonFlr) = "Interzone Floor";
                rowHead(rGrdFlr) = "Ground Contact Floor";
                rowHead(rOtherFlr) = "Other Floor";
                // subsurfaces
                rowHead(rFeneCond) = "Fenestration Conduction";
                rowHead(rFeneSolr) = "Fenestration Solar";
                rowHead(rOpqDoor) = "Opaque Door";
                rowHead(rGrdTot) = "Grand Total";

                columnHead.allocate(cPerArea);
                if (ort->unitsStyle != iUnitsStyle::InchPound) {
                    columnHead(cSensInst) = "Sensible - Instant [W]";
                    columnHead(cSensDelay) = "Sensible - Delayed [W]";
                    columnHead(cSensRA) = "Sensible - Return Air [W]";
                    columnHead(cLatent) = "Latent [W]";
                    columnHead(cTotal) = "Total [W]";
                    columnHead(cPerc) = "%Grand Total";
                    columnHead(cArea) = "Related Area [m2]";
                    columnHead(cPerArea) = "Total per Area [W/m2]";

                } else {
                    columnHead(cSensInst) = "Sensible - Instant [Btu/h]";
                    columnHead(cSensDelay) = "Sensible - Delayed [Btu/h]";
                    columnHead(cSensRA) = "Sensible - Return Air [Btu/h]";
                    columnHead(cLatent) = "Latent [Btu/h]";
                    columnHead(cTotal) = "Total [Btu/h]";
                    columnHead(cPerc) = "%Grand Total";
                    columnHead(cArea) = "Related Area [ft2]";
                    columnHead(cPerArea) = "Total per Area [Btu/h-ft2]";
                }
                columnWidth.dimension(cPerArea, 14); // array assignment - same for all columns

                WriteSubtitle(state, peakLoadCompName);
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(tableBody, rowHead, columnHead, reportName, zoneAirLoopFacilityName, peakLoadCompName);
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                        tableBody, rowHead, columnHead, reportName, zoneAirLoopFacilityName, peakLoadCompName);
                }

                //---- Peak Conditions

                rowHead.allocate(16);
                columnHead.allocate(1);
                columnWidth.allocate(1);
                columnWidth = 14; // array assignment - same for all columns

                tableBody.allocate(1, 16);
                tableBody = "";

                columnHead(1) = "Value";
                if (ort->unitsStyle != iUnitsStyle::InchPound) {
                    rowHead(1) = "Time of Peak Load";
                    rowHead(2) = "Outside Dry Bulb Temperature [C]";
                    rowHead(3) = "Outside Wet Bulb Temperature [C]";
                    rowHead(4) = "Outside Humidity Ratio at Peak [kgWater/kgDryAir]";
                    rowHead(5) = "Zone Dry Bulb Temperature [C]";
                    rowHead(6) = "Zone Relative Humidity [%]";
                    rowHead(7) = "Zone Humidity Ratio at Peak [kgWater/kgDryAir]";

                    rowHead(8) = "Supply Air Temperature [C]";
                    rowHead(9) = "Mixed Air Temperature [C]";
                    rowHead(10) = "Main Fan Air Flow [m3/s]";
                    rowHead(11) = "Outside Air Flow [m3/s]";
                    rowHead(12) = "Peak Sensible Load with Sizing Factor [W]";
                    rowHead(13) = "Difference Due to Sizing Factor [W]";

                    rowHead(14) = "Peak Sensible Load [W]";
                    rowHead(15) = "Estimated Instant + Delayed Sensible Load [W]";
                    rowHead(16) = "Difference Between Peak and Estimated Sensible Load [W]";
                } else {
                    rowHead(1) = "Time of Peak Load";
                    rowHead(2) = "Outside Dry Bulb Temperature [F]";
                    rowHead(3) = "Outside Wet Bulb Temperature [F]";
                    rowHead(4) = "Outside Humidity Ratio at Peak [lbWater/lbAir]";
                    rowHead(5) = "Zone Dry Bulb Temperature [F]";
                    rowHead(6) = "Zone Relative Humidity [%]";
                    rowHead(7) = "Zone Humidity Ratio at Peak [lbWater/lbAir]";

                    rowHead(8) = "Supply Air Temperature [F]";
                    rowHead(9) = "Mixed Air Temperature [F]";
                    rowHead(10) = "Main Fan Air Flow [ft3/min]";
                    rowHead(11) = "Outside Air Flow [ft3/min]";
                    rowHead(12) = "Peak Sensible Load with Sizing Factor [Btu/h]";
                    rowHead(13) = "Difference Due to Sizing Factor [Btu/h]";

                    rowHead(14) = "Peak Sensible Load  [Btu/h]";
                    rowHead(15) = "Estimated Instant + Delayed Sensible Load [Btu/h]";
                    rowHead(16) = "Difference Between Peak and Estimated Sensible Load [Btu/h]";
                }

                if (curCompLoad.timeStepMax != 0) {
                    tableBody(1, 1) = curCompLoad.peakDateHrMin;                  // Time of Peak Load
                    tableBody(1, 2) = RealToStr(curCompLoad.outsideDryBulb, 2);   // Outside Dry Bulb Temperature
                    tableBody(1, 3) = RealToStr(curCompLoad.outsideWetBulb, 2);   // Outside Wet Bulb Temperature
                    tableBody(1, 4) = RealToStr(curCompLoad.outsideHumRatio, 5);  // Outside Humidity Ratio at Peak
                    tableBody(1, 5) = RealToStr(curCompLoad.zoneDryBulb, 2);      // Zone Dry Bulb Temperature
                    tableBody(1, 6) = RealToStr(100 * curCompLoad.zoneRelHum, 2); // Zone Relative Humdity
                    tableBody(1, 7) = RealToStr(curCompLoad.zoneHumRatio, 5);     // Zone Humidity Ratio at Peak
                }
                tableBody(1, 8) = RealToStr(curCompLoad.supAirTemp, 2); // supply air temperature
                if (kind == iOutputType::airLoopOutput) {
                    tableBody(1, 9) = RealToStr(curCompLoad.mixAirTemp, 2); // mixed air temperature - not for zone or facility
                }
                tableBody(1, 10) = RealToStr(curCompLoad.mainFanAirFlow, 2);     // main fan air flow
                tableBody(1, 11) = RealToStr(curCompLoad.outsideAirFlow, 2);     // outside air flow
                tableBody(1, 12) = RealToStr(curCompLoad.designPeakLoad, 2);     // design peak load
                tableBody(1, 13) = RealToStr(curCompLoad.diffDesignPeak, 2);     // difference between Design and Peak Load
                tableBody(1, 14) = RealToStr(curCompLoad.peakDesSensLoad, 2);    // Peak Design Sensible Load
                tableBody(1, 15) = RealToStr(curCompLoad.estInstDelSensLoad, 2); // Estimated Instant + Delayed Sensible Load
                tableBody(1, 16) = RealToStr(curCompLoad.diffPeakEst, 2);        // Difference

                WriteSubtitle(state, peakCondName);
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(tableBody, rowHead, columnHead, reportName, zoneAirLoopFacilityName, peakCondName);
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                        tableBody, rowHead, columnHead, reportName, zoneAirLoopFacilityName, peakCondName);
                }

                //---- Engineering Checks

                rowHead.allocate(6);
                columnHead.allocate(1);
                columnWidth.allocate(1);
                columnWidth = 14; // array assignment - same for all columns

                tableBody.allocate(1, 6);
                tableBody = "";

                columnHead(1) = "Value";
                if (ort->unitsStyle != iUnitsStyle::InchPound) {
                    rowHead(1) = "Outside Air Fraction [fraction]";
                    rowHead(2) = "Airflow per Floor Area [m3/s-m2]";
                    rowHead(3) = "Airflow per Total Capacity [m3/s-W]";
                    rowHead(4) = "Floor Area per Total Capacity [m2/W]";
                    rowHead(5) = "Total Capacity per Floor Area [W/m2]";
                    // rowHead( 6 ) = "Chiller Pump Power per Flow [W-s/m3]"; // facility only
                    // rowHead( 7 ) = "Condenser Pump Power per Flor [W-s/m3]"; // facility only
                    rowHead(6) = "Number of People";
                } else {
                    rowHead(1) = "Outside Air Fraction [fraction]";
                    rowHead(2) = "Airflow per Floor Area [ft3/min-ft2]";
                    rowHead(3) = "Airflow per Total Capacity [ft3-h/min-Btu]";
                    rowHead(4) = "Floor Area per Total Capacity [ft2-h/Btu]";
                    rowHead(5) = "Total Capacity per Floor Area [Btu/h-ft2]";
                    // rowHead( 6 ) = "Chiller Pump Power per Flow [W-min/gal]";
                    // rowHead( 7 ) = "Condenser Pump Power per Flow [W-min/gal]";
                    rowHead(6) = "Number of People";
                }

                tableBody(1, 1) = fmt::format("{:.{}f}", curCompLoad.outsideAirRatio, 4); // outside Air
                tableBody(1, 2) = fmt::format("{:0.3E}", curCompLoad.airflowPerFlrArea);  // airflow per floor area
                tableBody(1, 3) = fmt::format("{:0.3E}", curCompLoad.airflowPerTotCap);   // airflow per total capacity
                tableBody(1, 4) = fmt::format("{:0.3E}", curCompLoad.areaPerTotCap);      // area per total capacity
                tableBody(1, 5) = fmt::format("{:0.3E}", curCompLoad.totCapPerArea);      // total capacity per area
                tableBody(1, 6) = fmt::format("{:.{}f}", curCompLoad.numPeople, 1);       // number of people

                WriteSubtitle(state, engineeringCheckName);
                WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                if (sqlite) {
                    sqlite->createSQLiteTabularDataRecords(tableBody, rowHead, columnHead, reportName, zoneAirLoopFacilityName, engineeringCheckName);
                }
                if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                    ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                        tableBody, rowHead, columnHead, reportName, zoneAirLoopFacilityName, engineeringCheckName);
                }

                // write the list of zone for the AirLoop level report
                if (kind == iOutputType::airLoopOutput && curCompLoad.zoneIndices.allocated()) {
                    int maxRow = 0;
                    for (size_t zi = 1; zi <= curCompLoad.zoneIndices.size(); ++zi) {
                        if (curCompLoad.zoneIndices(zi) > 0) {
                            maxRow = zi;
                        }
                    }

                    rowHead.allocate(maxRow);
                    columnHead.allocate(1);
                    columnWidth.allocate(1);
                    columnWidth = 14; // array assignment - same for all columns
                    tableBody.allocate(1, maxRow);
                    tableBody = "";

                    columnHead(1) = "Zone Name";
                    for (int zi = 1; zi <= maxRow; ++zi) {
                        rowHead(zi) = fmt::to_string(zi);
                        if (curCompLoad.zoneIndices(zi) > 0) {
                            tableBody(1, zi) = Zone(curCompLoad.zoneIndices(zi)).Name;
                        }
                    }

                    WriteSubtitle(state, zonesIncludedName);
                    WriteTable(state, tableBody, rowHead, columnHead, columnWidth);
                    if (sqlite) {
                        sqlite->createSQLiteTabularDataRecords(
                            tableBody, rowHead, columnHead, reportName, zoneAirLoopFacilityName, zonesIncludedName);
                    }
                    if (ResultsFramework::resultsFramework->timeSeriesAndTabularEnabled()) {
                        ResultsFramework::resultsFramework->TabularReportsCollection.addReportTable(
                            tableBody, rowHead, columnHead, reportName, zoneAirLoopFacilityName, zonesIncludedName);
                    }
                }
            }
        }
    }

    void WriteReportHeaders(EnergyPlusData &state, std::string const &reportName, std::string const &objectName, OutputProcessor::StoreType const averageOrSum)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   August 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Write the first few lines of each report with headers to the output
        //   file for tabular reports.
        // Using/Aliasing
        using DataHeatBalance::BuildingName;
        using DataStringGlobals::VerString;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:

        std::string const modifiedReportName(reportName + (averageOrSum == OutputProcessor::StoreType::Summed ? " per second" : ""));
        auto &ort(state.dataOutRptTab);

        for (int iStyle = 1; iStyle <= ort->numStyles; ++iStyle) {
            std::ostream &tbl_stream(*ort->TabularOutputFile(iStyle));
            std::string const &curDel(ort->del(iStyle));
            auto const style(ort->TableStyle(iStyle));
            if ((style == iTableStyle::Comma) || (style == iTableStyle::Tab)) {
                tbl_stream << "----------------------------------------------------------------------------------------------------\n";
                tbl_stream << "REPORT:" << curDel << modifiedReportName << '\n';
                tbl_stream << "FOR:" << curDel << objectName << '\n';
            } else if (style == iTableStyle::Fixed) {
                tbl_stream << "----------------------------------------------------------------------------------------------------\n";
                tbl_stream << "REPORT:      " << curDel << modifiedReportName << '\n';
                tbl_stream << "FOR:         " << curDel << objectName << '\n';
            } else if (style == iTableStyle::HTML) {
                tbl_stream << "<hr>\n";
                tbl_stream << "<p><a href=\"#toc\" style=\"float: right\">Table of Contents</a></p>\n";
                tbl_stream << "<a name=" << MakeAnchorName(reportName, objectName) << "></a>\n";
                tbl_stream << "<p>Report:<b>" << curDel << modifiedReportName << "</b></p>\n";
                tbl_stream << "<p>For:<b>" << curDel << objectName << "</b></p>\n";
                tbl_stream << "<p>Timestamp: <b>" << std::setw(4) << ort->td(1) << '-' << std::setfill('0') << std::setw(2) << ort->td(2) << '-' << std::setw(2)
                           << ort->td(3) << '\n';
                tbl_stream << "    " << std::setw(2) << ort->td(5) << ':' << std::setw(2) << ort->td(6) << ':' << std::setw(2) << ort->td(7) << std::setfill(' ')
                           << "</b></p>\n";
            } else if (style == iTableStyle::XML) {
                if (len(ort->prevReportName) != 0) {
                    tbl_stream << "</" << ort->prevReportName << ">\n"; // close the last element if it was used.
                }
                tbl_stream << "<" << ConvertToElementTag(modifiedReportName) << ">\n";
                tbl_stream << "  <for>" << objectName << "</for>\n";
                ort->prevReportName = ConvertToElementTag(modifiedReportName); // save the name for next time
            }
        }
        // clear the active subtable name for the XML reporting
        ort->activeSubTableName = "";
        // save the report name if the subtable name is not available during XML processing
        ort->activeReportName = modifiedReportName;
        // save the "for" which is the object name in the report for HTML comment that contains the report, for, and subtable
        ort->activeForName = objectName;
    }

    void WriteSubtitle(EnergyPlusData &state, std::string const &subtitle)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   November 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Insert a subtitle into the current report

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int iStyle;
        auto &ort(state.dataOutRptTab);

        for (iStyle = 1; iStyle <= ort->numStyles; ++iStyle) {
            auto const style(ort->TableStyle(iStyle));
            if ((style == iTableStyle::Comma) || (style == iTableStyle::Tab) || (style == iTableStyle::Fixed)) {
                std::ostream &tbl_stream(*ort->TabularOutputFile(iStyle));
                tbl_stream << subtitle << "\n\n";
            } else if (style == iTableStyle::HTML) {
                std::ostream &tbl_stream(*ort->TabularOutputFile(iStyle));
                tbl_stream << "<b>" << subtitle << "</b><br><br>\n";
                tbl_stream << "<!-- FullName:" << ort->activeReportName << '_' << ort->activeForName << '_' << subtitle << "-->\n";
            } else if (style == iTableStyle::XML) {
                // save the active subtable name for the XML reporting
                ort->activeSubTableName = subtitle;
                // no other output is needed since WriteTable uses the subtable name for each record.
            }
        }
    }

    void WriteTextLine(EnergyPlusData &state, std::string const &lineOfText, Optional_bool_const isBold)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   April 2007
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Insert a subtitle into the current report

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int iStyle;
        bool useBold;
        auto &ort(state.dataOutRptTab);

        if (present(isBold)) {
            useBold = isBold;
        } else {
            useBold = false;
        }

        for (iStyle = 1; iStyle <= ort->numStyles; ++iStyle) {
            auto const style(ort->TableStyle(iStyle));
            if ((style == iTableStyle::Comma) || (style == iTableStyle::Tab) || (style == iTableStyle::Fixed)) {
                std::ostream &tbl_stream(*ort->TabularOutputFile(iStyle));
                tbl_stream << lineOfText << '\n';
            } else if (style == iTableStyle::HTML) {
                std::ostream &tbl_stream(*ort->TabularOutputFile(iStyle));
                if (useBold) {
                    tbl_stream << "<b>" << lineOfText << "</b><br><br>\n";
                } else {
                    tbl_stream << lineOfText << "<br>\n";
                }
            } else if (style == iTableStyle::XML) {
                std::ostream &tbl_stream(*ort->TabularOutputFile(iStyle));
                if (!lineOfText.empty()) {
                    tbl_stream << "<note>" << lineOfText << "</note>\n";
                }
            }
        }
    }

    void WriteTable(EnergyPlusData &state,
                    Array2S_string const body, // row,column
                    const Array1D_string &rowLabels,
                    const Array1D_string &columnLabels,
                    Array1D_int &widthColumn,
                    Optional_bool_const transposeXML,
                    Optional_string_const footnoteText)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   August 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Output a table to the tabular output file in the selected
        //   style (comma, tab, space, html, xml).
        //   The widthColumn array is only used for fixed space formatted reports
        //   if columnLables contain a vertical bar '|', they are broken into multiple
        //   rows.  If they exceed the column width even after that and the format is
        //   fixed, they are further shortened.
        //   To include the currency symbol ($ by default but other symbols if the user
        //   has input it with Economics:CurrencyType) use the string ~~$~~ in the row
        //   headers, column headers, and body. For HTML files, the ASCII or UNICODE
        //   symbol for the currency will be included. For TXT files, the ASCII symbol
        //   will be used.

        // Argument array dimensioning

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        static std::string const blank;

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        Array2D_string colLabelMulti;
        std::string workColumn;
        Array1D_string rowLabelTags;
        Array1D_string columnLabelTags;
        Array1D_string rowUnitStrings;
        Array1D_string columnUnitStrings;
        Array2D_string bodyEsc;

        int numColLabelRows;
        int maxNumColLabelRows;
        std::string::size_type widthRowLabel;
        std::string::size_type maxWidthRowLabel;

        int rowsBody;
        int colsBody;
        int colsColumnLabels;
        int colsWidthColumn;
        int rowsRowLabels;

        int iCol;
        int jRow;
        int colWidthLimit;
        std::string::size_type barLoc;

        std::string outputLine;
        std::string spaces;
        int iStyle;
        std::string curDel;
        std::string tagWithAttrib;
        std::string::size_type col1start;
        bool doTransposeXML;
        bool isTableBlank;
        bool isRecordBlank;
        auto &ort(state.dataOutRptTab);

        if (present(transposeXML)) {
            doTransposeXML = transposeXML;
        } else {
            doTransposeXML = false; // if not present assume that the XML table should not be transposed
        }
        // create blank string
        spaces = blank; // REPEAT(' ',1000)
        // get sizes of arrays
        rowsBody = isize(body, 2);
        colsBody = isize(body, 1);
        rowsRowLabels = isize(rowLabels);
        colsColumnLabels = isize(columnLabels);
        colsWidthColumn = isize(widthColumn);
        // check size of arrays for consistancy and if inconsistent use smaller value
        // and display warning
        if (rowsBody != rowsRowLabels) {
            ShowWarningError(state, "REPORT:TABLE Inconsistant number of rows.");
            rowsBody = min(rowsBody, rowsRowLabels);
            rowsRowLabels = rowsBody;
        }
        if ((colsBody != colsColumnLabels) || (colsBody != colsWidthColumn)) {
            ShowWarningError(state, "REPORT:TABLE Inconsistant number of columns.");
            colsBody = min(colsBody, min(colsColumnLabels, colsWidthColumn));
            colsWidthColumn = colsBody;
            colsColumnLabels = colsBody;
        }
        // create arrays to hold the XML tags
        rowLabelTags.allocate(rowsBody);
        columnLabelTags.allocate(colsBody);
        rowUnitStrings.allocate(rowsBody);
        columnUnitStrings.allocate(colsBody);
        bodyEsc.allocate(colsBody, rowsBody);
        // create new array to hold multiple line column lables
        colLabelMulti.allocate(colsColumnLabels, 50);
        colLabelMulti = blank; // set array to blank
        numColLabelRows = 0;   // default value
        maxNumColLabelRows = 0;

        for (iStyle = 1; iStyle <= ort->numStyles; ++iStyle) {
            std::ostream &tbl_stream(*ort->TabularOutputFile(iStyle));
            curDel = ort->del(iStyle);
            // go through the columns and break them into multiple lines
            // if bar '|' is found in a row then break into two lines
            // if longer than the column width break into two lines for fixed style only
            for (iCol = 1; iCol <= colsColumnLabels; ++iCol) {
                numColLabelRows = 0;
                workColumn = columnLabels(iCol);
                widthColumn(iCol) = max(widthColumn(iCol), static_cast<int>(len(columnLabels(iCol))));
                while (true) {
                    barLoc = index(workColumn, '|');
                    if (barLoc != std::string::npos) {
                        ++numColLabelRows;
                        colLabelMulti(iCol, numColLabelRows) = workColumn.substr(0, barLoc);
                        workColumn.erase(0, barLoc + 1);
                    } else {
                        ++numColLabelRows;
                        colLabelMulti(iCol, numColLabelRows) = workColumn;
                        break; // inner do loop
                    }
                }
                if (numColLabelRows > maxNumColLabelRows) {
                    maxNumColLabelRows = numColLabelRows;
                }
            }
            // extra preprocessing for fixed style reports
            if (ort->TableStyle(iStyle) == iTableStyle::Fixed) {
                // break column headings into multiple rows if long (for fixed) or contain two spaces in a row.
                for (iCol = 1; iCol <= colsColumnLabels; ++iCol) {
                    colWidthLimit = widthColumn(iCol);
                    for (jRow = 1; jRow <= maxNumColLabelRows; ++jRow) {
                        pare(colLabelMulti(iCol, jRow), colWidthLimit);
                    }
                }
                maxWidthRowLabel = 0;
                for (jRow = 1; jRow <= rowsRowLabels; ++jRow) {
                    widthRowLabel = len(rowLabels(jRow));
                    if (widthRowLabel > maxWidthRowLabel) {
                        maxWidthRowLabel = widthRowLabel;
                    }
                }
            }

            // output depending on style of format
            auto const style(ort->TableStyle(iStyle));
            if ((style == iTableStyle::Comma) || (style == iTableStyle::Tab)) {
                // column headers
                for (jRow = 1; jRow <= maxNumColLabelRows; ++jRow) {
                    outputLine = curDel; // one leading delimiters on column header lines
                    for (iCol = 1; iCol <= colsColumnLabels; ++iCol) {
                        outputLine += curDel + stripped(colLabelMulti(iCol, jRow));
                    }
                    tbl_stream << InsertCurrencySymbol(state, outputLine, false) << '\n';
                }
                // body with row headers
                for (jRow = 1; jRow <= rowsBody; ++jRow) {
                    outputLine = curDel + rowLabels(jRow); // one leading delimiters on table body lines
                    for (iCol = 1; iCol <= colsBody; ++iCol) {
                        outputLine += curDel + stripped(body(iCol, jRow));
                    }
                    tbl_stream << InsertCurrencySymbol(state, outputLine, false) << '\n';
                }
                if (present(footnoteText)) {
                    if (!footnoteText().empty()) {
                        tbl_stream << footnoteText() << '\n';
                    }
                }
                tbl_stream << "\n\n";

            } else if (style == iTableStyle::Fixed) {
                // column headers
                for (jRow = 1; jRow <= maxNumColLabelRows; ++jRow) {
                    outputLine = blank; // spaces(:maxWidthRowLabel+2)  // two extra spaces and leave blank area for row labels
                    col1start = max(maxWidthRowLabel + 2u, static_cast<std::string::size_type>(3u));
                    for (iCol = 1; iCol <= colsColumnLabels; ++iCol) {
                        if (iCol != 1) {
                            outputLine += "  " + rjustified(sized(colLabelMulti(iCol, jRow), widthColumn(iCol)));
                        } else {
                            outputLine = std::string(col1start - 1, ' ') + "  " + rjustified(sized(colLabelMulti(iCol, jRow), widthColumn(iCol)));
                        }
                    }
                    tbl_stream << InsertCurrencySymbol(state, outputLine, false) << '\n';
                }
                // body with row headers
                for (jRow = 1; jRow <= rowsBody; ++jRow) {
                    outputLine = "  " + rjustified(sized(rowLabels(jRow), maxWidthRowLabel)); // two blank spaces on table body lines
                    // col1start = max( len( outputLine ) + 2u, maxWidthRowLabel + 2u );
                    for (iCol = 1; iCol <= colsBody; ++iCol) {
                        if (iCol != 1) {
                            outputLine += "  " + rjustified(sized(body(iCol, jRow), widthColumn(iCol)));
                        } else {
                            outputLine += "   " + rjustified(sized(body(iCol, jRow), widthColumn(iCol)));
                        }
                    }
                    tbl_stream << InsertCurrencySymbol(state, outputLine, false) << '\n';
                }
                if (present(footnoteText)) {
                    if (!footnoteText().empty()) {
                        tbl_stream << footnoteText() << '\n';
                    }
                }
                tbl_stream << "\n\n";

            } else if (style == iTableStyle::HTML) {
                // set up it being a table
                tbl_stream << "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\">\n";
                // column headers
                tbl_stream << "  <tr><td></td>\n"; // start new row and leave empty cell
                for (iCol = 1; iCol <= colsColumnLabels; ++iCol) {
                    outputLine = "    <td align=\"right\">";
                    for (jRow = 1; jRow <= maxNumColLabelRows; ++jRow) {
                        outputLine += colLabelMulti(iCol, jRow);
                        if (jRow < maxNumColLabelRows) {
                            outputLine += "<br>";
                        }
                    }
                    tbl_stream << InsertCurrencySymbol(state, outputLine, true) << "</td>\n";
                }
                tbl_stream << "  </tr>\n";
                // body with row headers
                for (jRow = 1; jRow <= rowsBody; ++jRow) {
                    tbl_stream << "  <tr>\n";
                    if (rowLabels(jRow) != "") {
                        tbl_stream << "    <td align=\"right\">" << InsertCurrencySymbol(state, rowLabels(jRow), true) << "</td>\n";
                    } else {
                        tbl_stream << "    <td align=\"right\">&nbsp;</td>\n";
                    }
                    for (iCol = 1; iCol <= colsBody; ++iCol) {
                        if (body(iCol, jRow) != "") {
                            tbl_stream << "    <td align=\"right\">" << InsertCurrencySymbol(state, body(iCol, jRow), true) << "</td>\n";
                        } else {
                            tbl_stream << "    <td align=\"right\">&nbsp;</td>\n";
                        }
                    }
                    tbl_stream << "  </tr>\n";
                }
                // end the table
                tbl_stream << "</table>\n";
                if (present(footnoteText)) {
                    if (!footnoteText().empty()) {
                        tbl_stream << "<i>" << footnoteText() << "</i>\n";
                    }
                }
                tbl_stream << "<br><br>\n";
            } else if (style == iTableStyle::XML) {
                // check if entire table is blank and it if is skip generating anything
                isTableBlank = true;
                for (jRow = 1; jRow <= rowsBody; ++jRow) {
                    for (iCol = 1; iCol <= colsBody; ++iCol) {
                        if (len(body(iCol, jRow)) > 0) {
                            isTableBlank = false;
                            break;
                        }
                    }
                    if (!isTableBlank) break;
                }
                // if non-blank cells in the table body were found create the table.
                if (!isTableBlank) {
                    // if report name and subtable name the same add "record" to the end
                    ort->activeSubTableName = ConvertToElementTag(ort->activeSubTableName);
                    ort->activeReportNameNoSpace = ConvertToElementTag(ort->activeReportName);
                    if (UtilityRoutines::SameString(ort->activeSubTableName, ort->activeReportNameNoSpace)) {
                        ort->activeSubTableName += "Record";
                    }
                    // if no subtable name use the report name and add "record" to the end
                    if (len(ort->activeSubTableName) == 0) {
                        ort->activeSubTableName = ort->activeReportNameNoSpace + "Record";
                    }
                    // if a single column table, transpose it automatically
                    if ((colsBody == 1) && (rowsBody > 1)) {
                        doTransposeXML = true;
                    }
                    // first convert all row and column headers into tags compatible with XML strings
                    for (jRow = 1; jRow <= rowsBody; ++jRow) {
                        rowLabelTags(jRow) = ConvertToElementTag(rowLabels(jRow));
                        if (len(rowLabelTags(jRow)) == 0) {
                            rowLabelTags(jRow) = "none";
                        }
                        rowUnitStrings(jRow) = GetUnitSubString(rowLabels(jRow));
                        if (UtilityRoutines::SameString(rowUnitStrings(jRow), "Invalid/Undefined")) {
                            rowUnitStrings(jRow) = "";
                        }
                    }
                    for (iCol = 1; iCol <= colsBody; ++iCol) {
                        columnLabelTags(iCol) = ConvertToElementTag(columnLabels(iCol));
                        if (len(columnLabelTags(iCol)) == 0) {
                            columnLabelTags(iCol) = "none";
                        }
                        columnUnitStrings(iCol) = GetUnitSubString(columnLabels(iCol));
                        if (UtilityRoutines::SameString(columnUnitStrings(iCol), "Invalid/Undefined")) {
                            columnUnitStrings(iCol) = "";
                        }
                    }
                    // convert entire table body to one with escape characters (no " ' < > &)
                    for (jRow = 1; jRow <= rowsBody; ++jRow) {
                        for (iCol = 1; iCol <= colsBody; ++iCol) {
                            bodyEsc(iCol, jRow) = ConvertToEscaped(body(iCol, jRow));
                        }
                    }
                    if (!doTransposeXML) {
                        // body with row headers
                        for (jRow = 1; jRow <= rowsBody; ++jRow) {
                            // check if record is blank and it if is skip generating anything
                            isRecordBlank = true;
                            for (iCol = 1; iCol <= colsBody; ++iCol) {
                                if (len(bodyEsc(iCol, jRow)) > 0) {
                                    isRecordBlank = false;
                                    break;
                                }
                            }
                            if (!isRecordBlank) {
                                tbl_stream << "  <" << ort->activeSubTableName << ">\n";
                                if (len(rowLabelTags(jRow)) > 0) {
                                    tbl_stream << "    <name>" << rowLabelTags(jRow) << "</name>\n";
                                }
                                for (iCol = 1; iCol <= colsBody; ++iCol) {
                                    if (len(stripped(bodyEsc(iCol, jRow))) > 0) { // skip blank cells
                                        tagWithAttrib = "<" + columnLabelTags(iCol);
                                        if (len(columnUnitStrings(iCol)) > 0) {
                                            tagWithAttrib += std::string(" units=") + char(34) + columnUnitStrings(iCol) + char(34) +
                                                             '>'; // if units are present add them as an attribute
                                        } else {
                                            tagWithAttrib += ">";
                                        }
                                        tbl_stream << "    " << tagWithAttrib << stripped(bodyEsc(iCol, jRow)) << "</" << columnLabelTags(iCol)
                                                   << ">\n";
                                    }
                                }
                                tbl_stream << "  </" << ort->activeSubTableName << ">\n";
                            }
                        }
                    } else { // transpose XML table
                        // body with row headers
                        for (iCol = 1; iCol <= colsBody; ++iCol) {
                            // check if record is blank and it if is skip generating anything
                            isRecordBlank = true;
                            for (jRow = 1; jRow <= rowsBody; ++jRow) {
                                if (len(bodyEsc(iCol, jRow)) > 0) {
                                    isRecordBlank = false;
                                    break;
                                }
                            }
                            if (!isRecordBlank) {
                                tbl_stream << "  <" << ort->activeSubTableName << ">\n";
                                // if the column has units put them into the name tag
                                if (len(columnLabelTags(iCol)) > 0) {
                                    if (len(columnUnitStrings(iCol)) > 0) {
                                        tbl_stream << "    <name units=" << char(34) << columnUnitStrings(iCol) << char(34) << '>'
                                                   << columnLabelTags(iCol) << "</name>\n";
                                    } else {
                                        tbl_stream << "    <name>" << columnLabelTags(iCol) << "</name>\n";
                                    }
                                }
                                for (jRow = 1; jRow <= rowsBody; ++jRow) {
                                    if (len(bodyEsc(iCol, jRow)) > 0) { // skip blank cells
                                        tagWithAttrib = "<" + rowLabelTags(jRow);
                                        if (len(rowUnitStrings(jRow)) > 0) {
                                            tagWithAttrib += std::string(" units=") + char(34) + rowUnitStrings(jRow) + char(34) +
                                                             ">\n"; // if units are present add them as an attribute
                                        } else {
                                            tagWithAttrib += ">";
                                        }
                                        tbl_stream << "    " << tagWithAttrib << stripped(bodyEsc(iCol, jRow)) << "</" << rowLabelTags(jRow) << ">\n";
                                    }
                                }
                                tbl_stream << "  </" << ort->activeSubTableName << ">\n";
                            }
                        }
                    }
                    if (present(footnoteText)) {
                        if (!footnoteText().empty()) {
                            tbl_stream << "  <footnote>" << footnoteText() << "</footnote>\n";
                        }
                    }
                }
            } else {
            }
        }
    }

    std::string MakeAnchorName(std::string const &reportString, std::string const &objectString)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   June 2005
        //       MODIFIED
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Use the name of the report and object be used to create and HTML anchor

        // METHODOLOGY EMPLOYED:
        //   Remove spaces and put double colon between names

        // REFERENCES:
        // na

        // USE STATEMENTS:

        // Return value
        std::string StringOut;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:

        for (std::string::size_type i = 0, e = reportString.length(); i < e; ++i) {
            if (has(validChars, reportString[i])) {
                StringOut += reportString[i];
            }
        }
        StringOut += "::";
        for (std::string::size_type i = 0, e = objectString.length(); i < e; ++i) {
            if (has(validChars, objectString[i])) {
                StringOut += objectString[i];
            }
        }
        return StringOut;
    }

    std::string InsertCurrencySymbol(EnergyPlusData &state,
                                     std::string const &inString, // Input String
                                     bool const isHTML            // True if an HTML string
    )
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   August 2008
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Looks for the ~~$~~

        // METHODOLOGY EMPLOYED:
        //   na
        // Using/Aliasing

        // Return value

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:

        std::string outSt(trimmed(inString)); // Result String
        std::string::size_type loc = index(outSt, "~~$~~");
        while (loc != std::string::npos) {
            if (isHTML) {
                outSt = inString.substr(0, loc) + state.dataCostEstimateManager->monetaryUnit(state.dataCostEstimateManager->selectedMonetaryUnit).html + outSt.substr(loc + 5);
            } else {
                outSt = inString.substr(0, loc) + state.dataCostEstimateManager->monetaryUnit(state.dataCostEstimateManager->selectedMonetaryUnit).txt + outSt.substr(loc + 5);
            }
            loc = index(outSt, "~~$~~");
        }
        return outSt;
    }

    std::string ConvertToElementTag(std::string const &inString) // Input String
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   February 2013
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Convert report column or row header into a tag string
        //   that just has A-Z, a-z, or 0-1 characters and is
        //   shown in camelCase.

        // METHODOLOGY EMPLOYED:
        //   na

        // Return value
        std::string outString; // Result String

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:

        bool foundOther = true; // flag if character found besides A-Z, a-z, 0-9
        for (std::string::size_type iIn = 0, e = inString.length(); iIn < e; ++iIn) {
            char const c(inString[iIn]);
            int const curCharVal = int(c);
            if ((curCharVal >= 65) && (curCharVal <= 90)) { // A-Z upper case
                if (foundOther) {
                    outString += c; // keep as upper case after finding a space or another character
                } else {
                    outString += char(curCharVal + 32); // convert to lower case
                }
                foundOther = false;
            } else if ((curCharVal >= 97) && (curCharVal <= 122)) { // A-Z lower case
                if (foundOther) {
                    outString += char(curCharVal - 32); // convert to upper case
                } else {
                    outString += c; // leave as lower case
                }
                foundOther = false;
            } else if ((curCharVal >= 48) && (curCharVal <= 57)) { // 0-9 numbers
                // if first character is a number then prepend with the letter "t"
                if (outString.length() == 0) outString += 't';
                outString += c;
                foundOther = false;
            } else if (curCharVal == 91) { // [ bracket
                break;                     // stop parsing because unit string was found
            } else {
                foundOther = true;
            }
        }
        return outString;
    }

    std::string ConvertUnicodeToUTF8(unsigned long const codepoint)
    {
        // Taken from http://stackoverflow.com/a/19968992/2358662 and http://stackoverflow.com/a/148766/2358662
        std::string s;
        if (codepoint <= 0x7f) {
            s.push_back(static_cast<char>(codepoint));
        } else if (codepoint <= 0x7ff) {
            s.push_back(static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
            s.push_back(static_cast<char>(0x80 | (codepoint & 0x3f)));
        } else if (codepoint <= 0xffff) {
            s.push_back(static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
            s.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
            s.push_back(static_cast<char>(0x80 | (codepoint & 0x3f)));
        } else if (codepoint <= 0x10ffff) {
            s.push_back(static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
            s.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
            s.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
            s.push_back(static_cast<char>(0x80 | (codepoint & 0x3f)));
        }
        return s;
    }

    std::string ConvertToEscaped(std::string const &inString) // Input String
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   February 2013
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Convert to XML safe escaped character string
        //   so it excludes:
        //               " ' < > &

        if (inString.empty()) return "";

        std::string s;
        auto const inputSize = inString.size();
        s.reserve(inputSize);
        size_t index(0);
        char c;

        while (true) {
            if (index == inputSize) break;
            c = inString[index++];
            if (c == '\"') {
                s += "&quot;";
            } else if (c == '&') {
                s += "&amp;";
            } else if (c == '\'') {
                s += "&apos;";
            } else if (c == '<') {
                s += "&lt;";
            } else if (c == '>') {
                s += "&gt;";
            } else if (c == char(176)) {
                s += "&deg;";
            } else if (c == '\xC2') {
                if (index == inputSize) {
                    s += '\xC2';
                } else {
                    c = inString[index++];
                    if (c == '\xB0') {
                        s += "&deg;";
                    } else {
                        s += '\xC2';
                        s += c;
                    }
                }
            } else if (c == '\xB0') {
                s += "&deg;";
            } else if (c == '\\') {
                if (index == inputSize) break;
                c = inString[index++];
                if (c == '"') {
                    s += "&quot;";
                } else if (c == '\'') {
                    s += "&apos;";
                } else if (c == 'u' || c == 'x') {
                    int remainingLen = inputSize - index;
                    unsigned long codePoint(0);
                    if (c == 'u' && remainingLen > 3) {
                        codePoint = std::stoul(inString.substr(index, 4), nullptr, 16);
                        index += 4;
                    } else if (c == 'x' && remainingLen > 1) {
                        codePoint = std::stoul(inString.substr(index, 2), nullptr, 16);
                        index += 2;
                    }
                    auto const unicodeString = ConvertUnicodeToUTF8(codePoint);
                    if (unicodeString == "\xC2\xB0") { // only check for degree at this point
                        s += "&deg;";
                    } else {
                        s += unicodeString;
                    }
                } else {
                    s += c;
                }
            } else {
                s += c;
            }
        }
        return s;
    }

    void DetermineBuildingFloorArea(EnergyPlusData &state)
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   November 2003
        //       MODIFIED       BTG added checks for plenums. Feb2004
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   To determine the total floor area of the building and the
        //   conditioned floor area of the building

        // METHODOLOGY EMPLOYED:
        //   Use the Zone array and sum the areas for all zones

        // REFERENCES:
        // na

        // Using/Aliasing
        using DataHeatBalance::Zone;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        Real64 curZoneArea;
        int iZone;
        auto &ort(state.dataOutRptTab);

        ort->buildingGrossFloorArea = 0.0;
        ort->buildingConditionedFloorArea = 0.0;
        for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
            curZoneArea = Zone(iZone).FloorArea * Zone(iZone).Multiplier * Zone(iZone).ListMultiplier;

            // OLD CHECK IF PLENUM SHOULD BE EXCLUDED AUTOMATICALLY
            // check if this zone is also a return plenum or a supply plenum
            // found = 0
            // if (NumZoneReturnPlenums > 0) THEN
            //  found = UtilityRoutines::FindItemInList(Zone(iZone)%Name, ZoneRetPlenCond%ZoneName, NumZoneReturnPlenums)
            // endif
            // IF (found /= 0)  curZoneArea = 0.0d0
            // found = 0
            // if (NumZoneSupplyPlenums > 0) THEN
            //  found = UtilityRoutines::FindItemInList(Zone(iZone)%Name, ZoneSupPlenCond%ZoneName, NumZoneSupplyPlenums)
            // endif
            // IF (found /= 0)  curZoneArea = 0.0d0

            if (Zone(iZone).isPartOfTotalArea) {
                ort->buildingGrossFloorArea += curZoneArea;
                // If a ZoneHVAC:EquipmentConnections is used for a zone then
                // it is considered conditioned. Also ZONE SUPPLY PLENUM and ZONE RETURN PLENUM are
                // also is considered conditioned.
                if (Zone(iZone).SystemZoneNodeNumber > 0) {
                    ort->buildingConditionedFloorArea += curZoneArea;
                }
            }
        }
    }

    void FillRowHead(Array1D_string &rowHead)
    {
        // Forward fill the blanks in rowHead (eg End use column)
        std::string currentEndUseName;
        for (size_t i = 1; i <= rowHead.size(); ++i) {
            std::string thisEndUseName = rowHead(i);
            if (thisEndUseName.empty()) {
                rowHead(i) = currentEndUseName;
            } else {
                currentEndUseName = thisEndUseName;
            }
        }
    }

    //======================================================================================================================
    //======================================================================================================================

    //    ROUTINES TO RESET GATHERED VALUES TO ZERO

    //======================================================================================================================
    //======================================================================================================================

    void ResetTabularReports(EnergyPlusData &state)
    {
        // Jason Glazer - October 2015
        // Reset all gathering arrays to zero for multi-year simulations
        // so that only last year is reported in tabular reports
        state.dataOutRptTab->gatherElapsedTimeBEPS = 0.0;
        ResetMonthlyGathering(state);
        OutputReportTabularAnnual::ResetAnnualGathering();
        ResetBinGathering(state);
        ResetBEPSGathering(state);
        ResetSourceEnergyEndUseGathering(state);
        ResetPeakDemandGathering(state);
        ResetHeatGainGathering(state);
        ResetRemainingPredefinedEntries(state);
        ThermalComfort::ResetThermalComfortSimpleASH55(state);
        ThermalComfort::ResetSetPointMet(state);
        ResetAdaptiveComfort(state);
        state.dataOutputProcessor->isFinalYear = true;
    }

    void ResetMonthlyGathering(EnergyPlusData &state)
    {
        // Jason Glazer - October 2015
        // Reset all monthly gathering arrays to zero for multi-year simulations
        // so that only last year is reported in tabular reports
        int iInput;
        int jTable;
        int kColumn;
        int curTable;
        int curCol;
        static Real64 BigNum(0.0);
        auto &ort(state.dataOutRptTab);

        for (iInput = 1; iInput <= ort->MonthlyInputCount; ++iInput) {
            for (jTable = 1; jTable <= ort->MonthlyInput(iInput).numTables; ++jTable) {
                curTable = jTable + ort->MonthlyInput(iInput).firstTable - 1;
                for (kColumn = 1; kColumn <= ort->MonthlyTables(curTable).numColumns; ++kColumn) {
                    curCol = kColumn + ort->MonthlyTables(curTable).firstColumn - 1;
                    ort->MonthlyColumns(curCol).timeStamp = 0;
                    ort->MonthlyColumns(curCol).duration = 0.0;
                    if (ort->MonthlyColumns(curCol).aggType == iAggType::Maximum || ort->MonthlyColumns(curCol).aggType == iAggType::MaximumDuringHoursShown) {
                        ort->MonthlyColumns(curCol).reslt = -HUGE_(BigNum);
                    } else if (ort->MonthlyColumns(curCol).aggType == iAggType::Minimum || ort->MonthlyColumns(curCol).aggType == iAggType::MinimumDuringHoursShown) {
                        ort->MonthlyColumns(curCol).reslt = HUGE_(BigNum);
                    } else {
                        ort->MonthlyColumns(curCol).reslt = 0.0;
                    }
                }
            }
        }
    }

    void ResetBinGathering(EnergyPlusData &state)
    {
        // Jason Glazer - October 2015
        // Reset all timebins gathering arrays to zero for multi-year simulations
        // so that only last year is reported in tabular reports
        Real64 const bigVal(0.0); // used with HUGE: Value doesn't matter, only type: Initialize so compiler doesn't warn about use uninitialized
        auto &ort(state.dataOutRptTab);

        // clear the binning arrays to zeros
        for (auto &e : ort->BinResults) {
            e.mnth = 0.0;
            e.hrly = 0.0;
        }
        for (auto &e : ort->BinResultsBelow) {
            e.mnth = 0.0;
            e.hrly = 0.0;
        }
        for (auto &e : ort->BinResultsAbove) {
            e.mnth = 0.0;
            e.hrly = 0.0;
        }

        // re-initialize statistics counters
        for (auto &e : ort->BinStatistics) {
            e.minimum = HUGE_(bigVal);
            e.maximum = -HUGE_(bigVal);
            e.n = 0;
            e.sum = 0.0;
            e.sum2 = 0.0;
        }
    }

    void ResetBEPSGathering(EnergyPlusData &state)
    {
        // Jason Glazer - October 2015
        // Reset all ABUPS gathering arrays to zero for multi-year simulations
        // so that only last year is reported in tabular reports
        auto &ort(state.dataOutRptTab);
        ort->gatherTotalsBEPS = 0.0;
        ort->gatherEndUseBEPS = 0.0;
        ort->gatherEndUseSubBEPS = 0.0;
        ort->gatherTotalsSource = 0.0;
        // reset the specific components being gathered
        ort->gatherPowerFuelFireGen = 0.0;
        ort->gatherPowerPV = 0.0;
        ort->gatherPowerWind = 0.0;
        ort->gatherPowerHTGeothermal = 0.0;
        ort->gatherElecProduced = 0.0;
        ort->gatherElecPurchased = 0.0;
        ort->gatherElecSurplusSold = 0.0;
        ort->gatherElecStorage = 0.0;
        ort->gatherPowerConversion = 0.0;
        ort->gatherWaterHeatRecovery = 0.0;
        ort->gatherAirHeatRecoveryCool = 0.0;
        ort->gatherAirHeatRecoveryHeat = 0.0;
        ort->gatherHeatHTGeothermal = 0.0;
        ort->gatherHeatSolarWater = 0.0;
        ort->gatherHeatSolarAir = 0.0;
        ort->gatherRainWater = 0.0;
        ort->gatherCondensate = 0.0;
        ort->gatherWellwater = 0.0;
        ort->gatherMains = 0.0;
        ort->gatherWaterEndUseTotal = 0.0;
    }

    void ResetSourceEnergyEndUseGathering(EnergyPlusData &state)
    {
        // Jason Glazer - October 2015
        // Reset all source energy end use table gathering arrays to zero for multi-year simulations
        // so that only last year is reported in tabular reports
        auto &ort(state.dataOutRptTab);
        ort->gatherTotalsBySourceBEPS = 0.0;
        ort->gatherEndUseBySourceBEPS = 0.0;
    }

    void ResetPeakDemandGathering(EnergyPlusData &state)
    {
        // Jason Glazer - October 2015
        // Reset all demand end use components table gathering arrays to zero for multi-year simulations
        // so that only last year is reported in tabular reports
        auto &ort(state.dataOutRptTab);
        ort->gatherDemandTotal = 0.0;
        ort->gatherDemandTimeStamp = 0;
        ort->gatherDemandEndUse = 0.0;
        ort->gatherDemandEndUseSub = 0.0;
    }

    void ResetHeatGainGathering(EnergyPlusData &state)
    {
        // Jason Glazer - October 2015
        // Reset all sensible heat gas summary report gathering arrays to zero for multi-year simulations
        // so that only last year is reported in tabular reports
        using DataHeatBalance::BuildingPreDefRep;
        using DataHeatBalance::ZonePreDefRep;
        int iZone;
        for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
            ZonePreDefRep(iZone).SHGSAnPeoplAdd = 0.;
            ZonePreDefRep(iZone).SHGSAnLiteAdd = 0.;
            ZonePreDefRep(iZone).SHGSAnZoneEqHt = 0.;
            ZonePreDefRep(iZone).SHGSAnZoneEqCl = 0.;
            ZonePreDefRep(iZone).SHGSAnIzaAdd = 0.;
            ZonePreDefRep(iZone).SHGSAnIzaRem = 0.;
            ZonePreDefRep(iZone).SHGSAnWindAdd = 0.;
            ZonePreDefRep(iZone).SHGSAnWindRem = 0.;
            ZonePreDefRep(iZone).SHGSAnInfilAdd = 0.;
            ZonePreDefRep(iZone).SHGSAnInfilRem = 0.;
            ZonePreDefRep(iZone).SHGSAnEquipAdd = 0.;
            ZonePreDefRep(iZone).SHGSAnEquipRem = 0.;
            ZonePreDefRep(iZone).SHGSAnHvacATUHt = 0.;
            ZonePreDefRep(iZone).SHGSAnHvacATUCl = 0.;
            ZonePreDefRep(iZone).SHGSAnSurfHt = 0.;
            ZonePreDefRep(iZone).SHGSAnSurfCl = 0.;
            ZonePreDefRep(iZone).SHGSAnOtherAdd = 0.;
            ZonePreDefRep(iZone).SHGSAnOtherRem = 0.;
            ZonePreDefRep(iZone).htPeak = 0.;
            ZonePreDefRep(iZone).htPtTimeStamp = 0;
            ZonePreDefRep(iZone).SHGSHtHvacHt = 0.;
            ZonePreDefRep(iZone).SHGSHtHvacCl = 0.;
            ZonePreDefRep(iZone).SHGSHtSurfHt = 0.;
            ZonePreDefRep(iZone).SHGSHtSurfCl = 0.;
            ZonePreDefRep(iZone).SHGSHtHvacATUHt = 0.;
            ZonePreDefRep(iZone).SHGSHtHvacATUCl = 0.;
            ZonePreDefRep(iZone).SHGSHtPeoplAdd = 0.;
            ZonePreDefRep(iZone).SHGSHtLiteAdd = 0.;
            ZonePreDefRep(iZone).SHGSHtEquipAdd = 0.;
            ZonePreDefRep(iZone).SHGSHtEquipRem = 0.;
            ZonePreDefRep(iZone).SHGSHtWindAdd = 0.;
            ZonePreDefRep(iZone).SHGSHtWindRem = 0.;
            ZonePreDefRep(iZone).SHGSHtIzaAdd = 0.;
            ZonePreDefRep(iZone).SHGSHtIzaRem = 0.;
            ZonePreDefRep(iZone).SHGSHtInfilAdd = 0.;
            ZonePreDefRep(iZone).SHGSHtInfilRem = 0.;
            ZonePreDefRep(iZone).SHGSHtOtherAdd = 0.;
            ZonePreDefRep(iZone).SHGSHtOtherRem = 0.;
            ZonePreDefRep(iZone).clPeak = 0.;
            ZonePreDefRep(iZone).clPtTimeStamp = 0;
            ZonePreDefRep(iZone).SHGSClHvacHt = 0.;
            ZonePreDefRep(iZone).SHGSClHvacCl = 0.;
            ZonePreDefRep(iZone).SHGSClSurfHt = 0.;
            ZonePreDefRep(iZone).SHGSClSurfCl = 0.;
            ZonePreDefRep(iZone).SHGSClHvacATUHt = 0.;
            ZonePreDefRep(iZone).SHGSClHvacATUCl = 0.;
            ZonePreDefRep(iZone).SHGSClPeoplAdd = 0.;
            ZonePreDefRep(iZone).SHGSClLiteAdd = 0.;
            ZonePreDefRep(iZone).SHGSClEquipAdd = 0.;
            ZonePreDefRep(iZone).SHGSClEquipRem = 0.;
            ZonePreDefRep(iZone).SHGSClWindAdd = 0.;
            ZonePreDefRep(iZone).SHGSClWindRem = 0.;
            ZonePreDefRep(iZone).SHGSClIzaAdd = 0.;
            ZonePreDefRep(iZone).SHGSClIzaRem = 0.;
            ZonePreDefRep(iZone).SHGSClInfilAdd = 0.;
            ZonePreDefRep(iZone).SHGSClInfilRem = 0.;
            ZonePreDefRep(iZone).SHGSClOtherAdd = 0.;
            ZonePreDefRep(iZone).SHGSClOtherRem = 0.;
        }

        BuildingPreDefRep.htPeak = 0.;
        BuildingPreDefRep.htPtTimeStamp = 0;
        BuildingPreDefRep.SHGSHtHvacHt = 0.0;
        BuildingPreDefRep.SHGSHtHvacCl = 0.0;
        BuildingPreDefRep.SHGSHtHvacATUHt = 0.0;
        BuildingPreDefRep.SHGSHtHvacATUCl = 0.0;
        BuildingPreDefRep.SHGSHtSurfHt = 0.0;
        BuildingPreDefRep.SHGSHtSurfCl = 0.0;
        BuildingPreDefRep.SHGSHtPeoplAdd = 0.0;
        BuildingPreDefRep.SHGSHtLiteAdd = 0.0;
        BuildingPreDefRep.SHGSHtEquipAdd = 0.0;
        BuildingPreDefRep.SHGSHtWindAdd = 0.0;
        BuildingPreDefRep.SHGSHtIzaAdd = 0.0;
        BuildingPreDefRep.SHGSHtInfilAdd = 0.0;
        BuildingPreDefRep.SHGSHtOtherAdd = 0.0;
        BuildingPreDefRep.SHGSHtEquipRem = 0.0;
        BuildingPreDefRep.SHGSHtWindRem = 0.0;
        BuildingPreDefRep.SHGSHtIzaRem = 0.0;
        BuildingPreDefRep.SHGSHtInfilRem = 0.0;
        BuildingPreDefRep.SHGSHtOtherRem = 0.0;

        BuildingPreDefRep.clPeak = 0.;
        BuildingPreDefRep.clPtTimeStamp = 0;
        BuildingPreDefRep.SHGSClHvacHt = 0.0;
        BuildingPreDefRep.SHGSClHvacCl = 0.0;
        BuildingPreDefRep.SHGSClSurfHt = 0.0;
        BuildingPreDefRep.SHGSClSurfCl = 0.0;
        BuildingPreDefRep.SHGSClHvacATUHt = 0.0;
        BuildingPreDefRep.SHGSClHvacATUCl = 0.0;
        BuildingPreDefRep.SHGSClPeoplAdd = 0.0;
        BuildingPreDefRep.SHGSClLiteAdd = 0.0;
        BuildingPreDefRep.SHGSClEquipAdd = 0.0;
        BuildingPreDefRep.SHGSClWindAdd = 0.0;
        BuildingPreDefRep.SHGSClIzaAdd = 0.0;
        BuildingPreDefRep.SHGSClInfilAdd = 0.0;
        BuildingPreDefRep.SHGSClOtherAdd = 0.0;
        BuildingPreDefRep.SHGSClEquipRem = 0.0;
        BuildingPreDefRep.SHGSClWindRem = 0.0;
        BuildingPreDefRep.SHGSClIzaRem = 0.0;
        BuildingPreDefRep.SHGSClInfilRem = 0.0;
        BuildingPreDefRep.SHGSClOtherRem = 0.0;
    }

    void ResetRemainingPredefinedEntries(EnergyPlusData &state)
    {
        // Jason Glazer - October 2015
        // Reset all entries that are added to the predefined reports in the FillRemainingPredefinedEntries() function to zero for multi-year
        // simulations so that only last year is reported in tabular reports
        using DataHeatBalance::Lights;
        using DataHeatBalance::TotLights;
        using DataHeatBalance::Zone;
        using DataHeatBalance::ZonePreDefRep;

        Real64 const bigVal(0.0); // used with HUGE: Value doesn't matter, only type: Initialize so compiler doesn't warn about use uninitialized
        int iLight;
        int iZone;

        for (iLight = 1; iLight <= TotLights; ++iLight) {
            Lights(iLight).SumTimeNotZeroCons = 0.;
            Lights(iLight).SumConsumption = 0.;
        }

        for (iZone = 1; iZone <= state.dataGlobal->NumOfZones; ++iZone) {
            if (Zone(iZone).SystemZoneNodeNumber >= 0) { // conditioned zones only
                if (Zone(iZone).isNominalOccupied) {
                    ZonePreDefRep(iZone).MechVentVolTotal = 0.;
                    ZonePreDefRep(iZone).MechVentVolMin = HUGE_(bigVal);
                    ZonePreDefRep(iZone).InfilVolTotal = 0.;
                    ZonePreDefRep(iZone).InfilVolMin = HUGE_(bigVal);
                    ZonePreDefRep(iZone).AFNInfilVolTotal = 0.;
                    ZonePreDefRep(iZone).AFNInfilVolMin = HUGE_(bigVal);
                    ZonePreDefRep(iZone).SimpVentVolTotal = 0.;
                    ZonePreDefRep(iZone).SimpVentVolMin = HUGE_(bigVal);
                    ZonePreDefRep(iZone).TotTimeOcc = 0.;
                }
            }
        }
    }

    void ResetAdaptiveComfort(EnergyPlusData &state)
    {
        // Jason Glazer - October 2015
        // Reset accumulation variable for adaptive comfort report to zero for multi-year simulations
        // so that only last year is reported in tabular reports
        using DataHeatBalance::People;
        using DataHeatBalance::TotPeople;
        int i;
        auto &ort(state.dataOutRptTab);
        if (ort->displayAdaptiveComfort && TotPeople > 0) {
            for (i = 1; i <= TotPeople; ++i) {
                if (People(i).AdaptiveASH55) {
                    People(i).TimeNotMetASH5590 = 0.;
                    People(i).TimeNotMetASH5580 = 0.;
                }
                if (People(i).AdaptiveCEN15251) {
                    People(i).TimeNotMetCEN15251CatI = 0.;
                    People(i).TimeNotMetCEN15251CatII = 0.;
                    People(i).TimeNotMetCEN15251CatIII = 0.;
                }
            }
        }
    }

    //======================================================================================================================
    //======================================================================================================================

    //    ROUTINES RELATED TO IF VALUE IS IN A RANGE

    //======================================================================================================================
    //======================================================================================================================

    bool isInTriangle(
        Real64 const qx, Real64 const qy, Real64 const x1, Real64 const y1, Real64 const x2, Real64 const y2, Real64 const x3, Real64 const y3)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   June 2005
        //       MODIFIED
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Determine if point q is in triangle defined by points a,b,c

        // METHODOLOGY EMPLOYED:
        //   The function used three times is positive the point is on the "right"
        //   side and negative if on "left" side. By checking to make sure the signs
        //   are always the same. it determines that the point is inside of the
        //   triangle.

        // REFERENCES:
        //   http://mcraefamily.com/MathHelp/GeometryPointAndTriangle2.htm

        // USE STATEMENTS:

        // Return value
        bool isInTriangle;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        Real64 fAB;
        Real64 fCA;
        Real64 fBC;

        fAB = (qy - y1) * (x2 - x1) - (qx - x1) * (y2 - y1);
        fCA = (qy - y3) * (x1 - x3) - (qx - x3) * (y1 - y3);
        fBC = (qy - y2) * (x3 - x2) - (qx - x2) * (y3 - y2);
        if ((fAB * fBC) >= 0.0 && (fBC * fCA) >= 0.0) {
            isInTriangle = true;
        } else {
            isInTriangle = false;
        }
        return isInTriangle;
    }

    bool isInQuadrilateral(Real64 const qx,
                           Real64 const qy,
                           Real64 const ax,
                           Real64 const ay,
                           Real64 const bx,
                           Real64 const by,
                           Real64 const cx,
                           Real64 const cy,
                           Real64 const dx,
                           Real64 const dy)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   June 2005
        //       MODIFIED
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Determine if point q is in a quadrilateral defined by points a,b,c,d
        //   Those points should express a quadrilateral in order of the points going
        //   around the outside of the polygon. They should not describe an "hourglass"
        //   shape where the lines cross in the middle of the figure.

        // METHODOLOGY EMPLOYED:
        //   Check if the point is in triangle a,b,c or in triangle c,d,a

        // REFERENCES:
        //   http://mcraefamily.com/MathHelp/GeometryPointAndTriangle4.htm

        // USE STATEMENTS:

        // Return value
        bool isInQuadrilateral;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:
        // na

        // SUBROUTINE PARAMETER DEFINITIONS:

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        bool inABC;
        bool inCDA;

        inABC = isInTriangle(qx, qy, ax, ay, bx, by, cx, cy);
        inCDA = isInTriangle(qx, qy, cx, cy, dx, dy, ax, ay);
        if (inABC || inCDA) {
            isInQuadrilateral = true;
        } else {
            isInQuadrilateral = false;
        }
        return isInQuadrilateral;
    }

    //======================================================================================================================
    //======================================================================================================================

    //    SUPPORT ROUTINES

    //======================================================================================================================
    //======================================================================================================================

    std::string RealToStr(Real64 const RealIn, int const numDigits)
    {
        // FUNCTION INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   August 2003
        //       MODIFIED       November 2008; LKL - prevent errors
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS FUNCTION:
        //   Abstract away the internal write concept

        // METHODOLOGY EMPLOYED:
        // na

        // REFERENCES:
        // na

        // USE STATEMENTS:
        // na

        // Return value
        std::string StringOut;

        // Locals
        // FUNCTION ARGUMENT DEFINITIONS:

        // FUNCTION PARAMETER DEFINITIONS:
        static constexpr std::array<const char *, 10> formDigitsA{
            "{:#12.0F}", "{:12.1F}", "{:12.2F}", "{:12.3F}", "{:12.4F}", "{:12.5F}", "{:12.6F}", "{:12.7F}", "{:12.8F}", "{:12.9F}"};

        static constexpr std::array<Real64, 10> maxvalDigitsA(
            {9999999999.0, 999999999.0, 99999999.0, 9999999.0, 999999.0, 99999.0, 9999.0, 999.0, 99.0, 9.0});


        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // FUNCTION LOCAL VARIABLE DECLARATIONS:
        int nDigits = numDigits;
        if (RealIn < 0.0) --nDigits;
        if (nDigits > 9) nDigits = 9;
        if (nDigits < 0) nDigits = 0;

        if (std::abs(RealIn) > maxvalDigitsA.at(nDigits)) {
            return format("{:12.6Z}", RealIn);
        } else {
            return format(formDigitsA.at(nDigits), RealIn);
        }
        //  WRITE(FMT=, UNIT=stringOut) RealIn
        // check if it did not fit
        //  IF (stringOut(1:1) .EQ. "*") THEN
        //    WRITE(FMT='(E12.6)', UNIT=stringOut) RealIn
        //  END IF

        // WRITE(FMT="(F10.4)", UNIT=stringOut, IOSTAT=status ) RealIn
    }

    Real64 StrToReal(std::string const &stringIn)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Linda Lawrie
        //       DATE WRITTEN   March 2010
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Abstract away the internal read concept

        // Return value
        Real64 realValue;

        std::stringstream ss{stringIn};
        ss.imbue(std::locale("C"));
        ss >> realValue;

        if (ss.bad()) {
            return -99999.0;
        }
        return realValue;
    }

    std::string DateToString(int const codedDate) // word containing encoded month, day, hour, minute
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   August 2003
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Convert the coded date format into a usable
        //   string

        // Using/Aliasing
        using General::DecodeMonDayHrMin;

        // Return value
        std::string StringOut;

        // Locals
        // ((month*100 + day)*100 + hour)*100 + minute

        int Month;  // month in integer format (1-12)
        int Day;    // day in integer format (1-31)
        int Hour;   // hour in integer format (1-24)
        int Minute; // minute in integer format (0:59)
        std::string monthName;

        if (codedDate != 0) {
            DecodeMonDayHrMin(codedDate, Month, Day, Hour, Minute);
            --Hour;
            if (Minute == 60) {
                ++Hour;
                Minute = 0;
            }
            if (Month == 1) {
                monthName = "JAN";
            } else if (Month == 2) {
                monthName = "FEB";
            } else if (Month == 3) {
                monthName = "MAR";
            } else if (Month == 4) {
                monthName = "APR";
            } else if (Month == 5) {
                monthName = "MAY";
            } else if (Month == 6) {
                monthName = "JUN";
            } else if (Month == 7) {
                monthName = "JUL";
            } else if (Month == 8) {
                monthName = "AUG";
            } else if (Month == 9) {
                monthName = "SEP";
            } else if (Month == 10) {
                monthName = "OCT";
            } else if (Month == 11) {
                monthName = "NOV";
            } else if (Month == 12) {
                monthName = "DEC";
            } else {
                monthName = "***";
            }
            StringOut = format("{:02}-{:3}-{:02}:{:02}", Day, monthName, Hour, Minute);
            if (has(StringOut, "*")) {
                StringOut = "-";
            }
        } else { // codeddate = 0
            StringOut = "-";
        }

        return StringOut;
    }

    bool isNumber(std::string const &s)
    {
        char *p;
        strtod(s.c_str(), &p);
        for (; isspace(*p); ++p)
            ; // handle trailing whitespace
        return *p == 0;
    }

    // return the number of digits after the decimal point
    // Glazer - November 2016
    int digitsAferDecimal(std::string s)
    {
        std::size_t decimalpos = s.find('.');
        std::size_t numDigits;
        if (decimalpos == s.npos) {
            numDigits = 0;
        } else {
            std::size_t epos = s.find('E');
            if (epos == s.npos) epos = s.find('e');
            if (epos == s.npos) {
                numDigits = s.length() - (decimalpos + 1);
            } else {
                numDigits = epos - (decimalpos + 1);
            }
        }
        return int(numDigits);
    }

    void AddTOCEntry(EnergyPlusData &state, std::string const &nameSection, std::string const &nameReport)
    {
        // SUBROUTINE INFORMATION:
        //    AUTHOR         Jason Glazer of GARD Analytics, Inc.
        //    DATE WRITTEN   September 2005
        //    MODIFIED       na
        //    RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Adds an entry for the TOC so that it can be created
        //   prior to the actual reports being generated. Note that
        //   the arguments must match what is used in
        //   "WriteReportHeaders" for the HTML anchors to work
        //   correctly.

        // METHODOLOGY EMPLOYED:

        // REFERENCES:
        //    na

        // USE STATEMENTS:

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        //    na

        // INTERFACE BLOCK SPECIFICATIONS:
        //    na

        // DERIVED TYPE DEFINITIONS:
        //    na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        //    na
        auto &ort(state.dataOutRptTab);

        if (!allocated(ort->TOCEntries)) {
            ort->TOCEntriesSize = 20;
            ort->TOCEntries.allocate(ort->TOCEntriesSize);
            ort->TOCEntriesCount = 1;
        } else {
            ++ort->TOCEntriesCount;
            // if larger than current size grow the array
            if (ort->TOCEntriesCount > ort->TOCEntriesSize) {
                ort->TOCEntries.redimension(ort->TOCEntriesSize += 20);
            }
        }
        ort->TOCEntries(ort->TOCEntriesCount).reportName = nameReport;
        ort->TOCEntries(ort->TOCEntriesCount).sectionName = nameSection;
    }

    void SetupUnitConversions(EnergyPlusData &state)
    {
        // SUBROUTINE INFORMATION:
        //    AUTHOR         Jason Glazer of GARD Analytics, Inc.
        //    DATE WRITTEN   February 12, 2009
        //    MODIFIED       March 2010; Linda Lawrie; Add deltaC and KJ/KG
        //    RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Initialize the array that contains the unit conversion
        //   information. The code is based on code generated
        //   in a spreadsheet titled UnitConversion.xls

        // METHODOLOGY EMPLOYED:

        // REFERENCES:
        //    na

        // USE STATEMENTS:

        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        //    na

        // INTERFACE BLOCK SPECIFICATIONS:
        //    na

        // DERIVED TYPE DEFINITIONS:
        //    na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        //    na
        auto &ort(state.dataOutRptTab);

        ort->UnitConvSize = 115;
        ort->UnitConv.allocate(ort->UnitConvSize);
        ort->UnitConv(1).siName = "%";
        ort->UnitConv(2).siName = "°C";
        ort->UnitConv(3).siName = "0=OFF 1=ON";
        ort->UnitConv(4).siName = "0-NO  1-YES";
        ort->UnitConv(5).siName = "1-YES 0-NO";
        ort->UnitConv(6).siName = "A";
        ort->UnitConv(7).siName = "ACH";
        ort->UnitConv(8).siName = "ACH";
        ort->UnitConv(9).siName = "BASE 10C";
        ort->UnitConv(10).siName = "BASE 18C";
        ort->UnitConv(11).siName = "C";
        ort->UnitConv(12).siName = "CD/M2";
        ort->UnitConv(13).siName = "DEG";
        ort->UnitConv(14).siName = "FRAC";
        ort->UnitConv(15).siName = "HOUR";
        ort->UnitConv(16).siName = "HOURS";
        ort->UnitConv(17).siName = "HR";
        ort->UnitConv(18).siName = "HRS";
        ort->UnitConv(19).siName = "J";
        ort->UnitConv(20).siName = "J";
        ort->UnitConv(21).siName = "J";
        ort->UnitConv(22).siName = "J";
        ort->UnitConv(23).siName = "J";
        ort->UnitConv(24).siName = "J";
        ort->UnitConv(25).siName = "J/KG";
        ort->UnitConv(26).siName = "J/KGWATER";
        ort->UnitConv(27).siName = "J/M2";
        ort->UnitConv(28).siName = "K/M";
        ort->UnitConv(29).siName = "KG";
        ort->UnitConv(30).siName = "KG/KG";
        ort->UnitConv(31).siName = "KG/M3";
        ort->UnitConv(32).siName = "KG/S";
        ort->UnitConv(33).siName = "KGWATER/KGDRYAIR";
        ort->UnitConv(34).siName = "KGWATER/SEC";
        ort->UnitConv(35).siName = "KMOL/S";
        ort->UnitConv(36).siName = "KMOL/SEC";
        ort->UnitConv(37).siName = "KWH";
        ort->UnitConv(38).siName = "L";
        ort->UnitConv(39).siName = "L";
        ort->UnitConv(40).siName = "LUM/W";
        ort->UnitConv(41).siName = "LUX";
        ort->UnitConv(42).siName = "M";
        ort->UnitConv(43).siName = "M";
        ort->UnitConv(44).siName = "M/S";
        ort->UnitConv(45).siName = "M/S";
        ort->UnitConv(46).siName = "M2";
        ort->UnitConv(47).siName = "M2/PERSON";
        ort->UnitConv(48).siName = "M3";
        ort->UnitConv(49).siName = "M3";
        ort->UnitConv(50).siName = "M3/M2";
        ort->UnitConv(51).siName = "M3/S";
        ort->UnitConv(52).siName = "M3/S";
        ort->UnitConv(53).siName = "M3/S-M2";
        ort->UnitConv(54).siName = "M3/S-PERSON";
        ort->UnitConv(55).siName = "M3/S-PERSON";
        ort->UnitConv(56).siName = "PA";
        ort->UnitConv(57).siName = "PA";
        ort->UnitConv(58).siName = "PA";
        ort->UnitConv(59).siName = "PA";
        ort->UnitConv(60).siName = "PA";
        ort->UnitConv(61).siName = "PA";
        ort->UnitConv(62).siName = "PA";
        ort->UnitConv(63).siName = "PA";
        ort->UnitConv(64).siName = "S";
        ort->UnitConv(65).siName = "V";
        ort->UnitConv(66).siName = "W";
        ort->UnitConv(67).siName = "W";
        ort->UnitConv(68).siName = "W";
        ort->UnitConv(69).siName = "W";
        ort->UnitConv(70).siName = "W";
        ort->UnitConv(71).siName = "W/KG";
        ort->UnitConv(72).siName = "W/KG H2O"; // TODO: replace with W/kgWater? or rather just remove
        ort->UnitConv(73).siName = "W/K";
        ort->UnitConv(74).siName = "W/M2";
        ort->UnitConv(75).siName = "W/M2";
        ort->UnitConv(76).siName = "W/M2-C";
        ort->UnitConv(77).siName = "W/M2-K";
        ort->UnitConv(78).siName = "W/W";
        ort->UnitConv(79).siName = "W/W";
        ort->UnitConv(80).siName = "deltaC";
        ort->UnitConv(81).siName = "KJ/KG";
        ort->UnitConv(82).siName = "W-S/M3";
        ort->UnitConv(83).siName = "W-S/M3";
        ort->UnitConv(84).siName = "~~$~~/m2";
        ort->UnitConv(85).siName = "GJ";
        ort->UnitConv(86).siName = "GJ";
        ort->UnitConv(87).siName = "GJ";
        ort->UnitConv(88).siName = "GJ";
        ort->UnitConv(89).siName = "GJ";
        ort->UnitConv(90).siName = "GJ";
        ort->UnitConv(91).siName = "GJ";
        ort->UnitConv(92).siName = "MJ/m2";
        ort->UnitConv(93).siName = "MJ/m2";
        ort->UnitConv(94).siName = "MJ/m2";
        ort->UnitConv(95).siName = "MJ/m2";
        ort->UnitConv(96).siName = "Invalid/Undefined";
        ort->UnitConv(97).siName = "";
        ort->UnitConv(98).siName = "W/C";
        ort->UnitConv(99).siName = "DAY";
        ort->UnitConv(100).siName = "MIN";
        ort->UnitConv(101).siName = "HR/WK";
        ort->UnitConv(102).siName = "$";
        ort->UnitConv(103).siName = "$/UNIT ENERGY";
        ort->UnitConv(104).siName = "KW";
        ort->UnitConv(105).siName = " ";
        ort->UnitConv(106).siName = "AH";
        ort->UnitConv(107).siName = "CLO";
        ort->UnitConv(108).siName = "J/KG-K";
        ort->UnitConv(109).siName = "KGWATER/S";
        ort->UnitConv(110).siName = "PPM";
        ort->UnitConv(111).siName = "RAD";
        ort->UnitConv(112).siName = "REV/MIN";
        ort->UnitConv(113).siName = "NM";
        ort->UnitConv(114).siName = "BTU/W-H"; // Used for AHRI rating metrics (e.g. SEER)
        ort->UnitConv(115).siName = "PERSON/M2";

        ort->UnitConv(1).ipName = "%";
        ort->UnitConv(2).ipName = "F";
        ort->UnitConv(3).ipName = "0=Off 1=On";
        ort->UnitConv(4).ipName = "0-No  1-Yes";
        ort->UnitConv(5).ipName = "1-Yes 0-No";
        ort->UnitConv(6).ipName = "A";
        ort->UnitConv(7).ipName = "ACH";
        ort->UnitConv(8).ipName = "ach";
        ort->UnitConv(9).ipName = "base 50F";
        ort->UnitConv(10).ipName = "base 65F";
        ort->UnitConv(11).ipName = "F";
        ort->UnitConv(12).ipName = "cd/in2";
        ort->UnitConv(13).ipName = "deg";
        ort->UnitConv(14).ipName = "Frac";
        ort->UnitConv(15).ipName = "Hour";
        ort->UnitConv(16).ipName = "Hours";
        ort->UnitConv(17).ipName = "hr";
        ort->UnitConv(18).ipName = "hrs";
        ort->UnitConv(19).ipName = "kBtu";
        ort->UnitConv(20).ipName = "kWh";
        ort->UnitConv(21).ipName = "therm";
        ort->UnitConv(22).ipName = "MMBtu";
        ort->UnitConv(23).ipName = "Wh";
        ort->UnitConv(24).ipName = "ton-hrs";
        ort->UnitConv(25).ipName = "Btu/lb";
        ort->UnitConv(26).ipName = "Btu/lbWater";
        ort->UnitConv(27).ipName = "kBtu/sqft";
        ort->UnitConv(28).ipName = "F/ft";
        ort->UnitConv(29).ipName = "lb";
        ort->UnitConv(30).ipName = "lb/lb";
        ort->UnitConv(31).ipName = "lb/ft3";
        ort->UnitConv(32).ipName = "lb/s";
        ort->UnitConv(33).ipName = "lbWater/lbDryAir";
        ort->UnitConv(34).ipName = "lbWater/s";
        ort->UnitConv(35).ipName = "kmol/s";
        ort->UnitConv(36).ipName = "kmol/sec";
        ort->UnitConv(37).ipName = "kWh";
        ort->UnitConv(38).ipName = "gal";
        ort->UnitConv(39).ipName = "ft3";
        ort->UnitConv(40).ipName = "lum/W";
        ort->UnitConv(41).ipName = "foot-candles";
        ort->UnitConv(42).ipName = "ft";
        ort->UnitConv(43).ipName = "in";
        ort->UnitConv(44).ipName = "ft/min";
        ort->UnitConv(45).ipName = "miles/hr";
        ort->UnitConv(46).ipName = "ft2";
        ort->UnitConv(47).ipName = "ft2/person";
        ort->UnitConv(48).ipName = "ft3";
        ort->UnitConv(49).ipName = "gal";
        ort->UnitConv(50).ipName = "f3/f2";
        ort->UnitConv(51).ipName = "ft3/min";
        ort->UnitConv(52).ipName = "gal/min";
        ort->UnitConv(53).ipName = "ft3/min-ft2";
        ort->UnitConv(54).ipName = "ft3/min-person";
        ort->UnitConv(55).ipName = "gal/min-person";
        ort->UnitConv(56).ipName = "psi";
        ort->UnitConv(57).ipName = "inHg";
        ort->UnitConv(58).ipName = "inH2O";
        ort->UnitConv(59).ipName = "ftH2O";
        ort->UnitConv(60).ipName = "psi";
        ort->UnitConv(61).ipName = "inHg";
        ort->UnitConv(62).ipName = "inH2O";
        ort->UnitConv(63).ipName = "ftH2O";
        ort->UnitConv(64).ipName = "s";
        ort->UnitConv(65).ipName = "V";
        ort->UnitConv(66).ipName = "Btu/h";
        ort->UnitConv(67).ipName = "W";
        ort->UnitConv(68).ipName = "kW";
        ort->UnitConv(69).ipName = "kBtuh";
        ort->UnitConv(70).ipName = "ton";
        ort->UnitConv(71).ipName = "kBtuh/lb";
        ort->UnitConv(72).ipName = "kBtuh/lb";
        ort->UnitConv(73).ipName = "Btu/h-F";
        ort->UnitConv(74).ipName = "Btu/h-ft2";
        ort->UnitConv(75).ipName = "kBtuh/ft2";
        ort->UnitConv(76).ipName = "Btu/h-ft2-F";
        ort->UnitConv(77).ipName = "Btu/h-ft2-F";
        ort->UnitConv(78).ipName = "Btuh/Btuh";
        ort->UnitConv(79).ipName = "W/W";
        ort->UnitConv(80).ipName = "deltaF";
        ort->UnitConv(81).ipName = "Btu/lb";
        ort->UnitConv(82).ipName = "W-min/ft3";
        ort->UnitConv(83).ipName = "W-min/gal";
        ort->UnitConv(84).ipName = "~~$~~/ft2";
        ort->UnitConv(85).ipName = "kBtu";
        ort->UnitConv(86).ipName = "kWh";
        ort->UnitConv(87).ipName = "kWh";
        ort->UnitConv(88).ipName = "therm";
        ort->UnitConv(89).ipName = "MMBtu";
        ort->UnitConv(90).ipName = "Wh";
        ort->UnitConv(91).ipName = "ton-hrs";
        ort->UnitConv(92).ipName = "kWh/ft2";
        ort->UnitConv(93).ipName = "kBtu/ft2";
        ort->UnitConv(94).ipName = "kBtu/ft2";
        ort->UnitConv(95).ipName = "kWh/m2";
        ort->UnitConv(96).ipName = "Invalid/Undefined";
        ort->UnitConv(97).ipName = "";
        ort->UnitConv(98).ipName = "Btu/h-F";
        ort->UnitConv(99).ipName = "day";
        ort->UnitConv(100).ipName = "min";
        ort->UnitConv(101).ipName = "hr/wk";
        ort->UnitConv(102).ipName = "$";
        ort->UnitConv(103).ipName = "$/unit energy";
        ort->UnitConv(104).ipName = "kW";
        ort->UnitConv(105).ipName = " ";
        ort->UnitConv(106).ipName = "Ah";
        ort->UnitConv(107).ipName = "clo";
        ort->UnitConv(108).ipName = "Btu/lbm-R";
        ort->UnitConv(109).ipName = "lbWater/s";
        ort->UnitConv(110).ipName = "ppm";
        ort->UnitConv(111).ipName = "rad";
        ort->UnitConv(112).ipName = "rev/min";
        ort->UnitConv(113).ipName = "lbf-ft";
        ort->UnitConv(114).ipName = "Btu/W-h";
        ort->UnitConv(115).ipName = "person/ft2";

        ort->UnitConv(1).mult = 1.0;
        ort->UnitConv(2).mult = 1.8;
        ort->UnitConv(3).mult = 1.0;
        ort->UnitConv(4).mult = 1.0;
        ort->UnitConv(5).mult = 1.0;
        ort->UnitConv(6).mult = 1.0;
        ort->UnitConv(7).mult = 1.0;
        ort->UnitConv(8).mult = 1.0;
        ort->UnitConv(9).mult = 1.8;
        ort->UnitConv(10).mult = 1.8;
        ort->UnitConv(11).mult = 1.8;
        ort->UnitConv(12).mult = 0.000645160041625726;
        ort->UnitConv(13).mult = 1.0;
        ort->UnitConv(14).mult = 1.0;
        ort->UnitConv(15).mult = 1.0;
        ort->UnitConv(16).mult = 1.0;
        ort->UnitConv(17).mult = 1.0;
        ort->UnitConv(18).mult = 1.0;
        ort->UnitConv(19).mult = 0.00000094845;
        ort->UnitConv(20).mult = 0.000000277778;
        ort->UnitConv(21).mult = 0.0000000094845;
        ort->UnitConv(22).mult = 0.00000000094845;
        ort->UnitConv(23).mult = 0.000277777777777778;
        ort->UnitConv(24).mult = 0.0000000789847;
        ort->UnitConv(25).mult = 0.00042956;
        ort->UnitConv(26).mult = 0.0000004302105;
        ort->UnitConv(27).mult = 0.00000008811404;
        ort->UnitConv(28).mult = 0.54861322767449;
        ort->UnitConv(29).mult = 2.2046;
        ort->UnitConv(30).mult = 1.0;
        ort->UnitConv(31).mult = 0.062428;
        ort->UnitConv(32).mult = 2.2046;
        ort->UnitConv(33).mult = 1.0;
        ort->UnitConv(34).mult = 2.2046;
        ort->UnitConv(35).mult = 1.0;
        ort->UnitConv(36).mult = 1.0;
        ort->UnitConv(37).mult = 1.0;
        ort->UnitConv(38).mult = 0.264172037284185;
        ort->UnitConv(39).mult = 0.0353146624712848;
        ort->UnitConv(40).mult = 1.0;
        ort->UnitConv(41).mult = 0.092902267;
        ort->UnitConv(42).mult = 3.281;
        ort->UnitConv(43).mult = 39.37;
        ort->UnitConv(44).mult = 196.86;
        ort->UnitConv(45).mult = 2.2369;
        ort->UnitConv(46).mult = 10.764961;
        ort->UnitConv(47).mult = 10.764961;
        ort->UnitConv(48).mult = 35.319837041;
        ort->UnitConv(49).mult = 264.172;
        ort->UnitConv(50).mult = 3.281;
        ort->UnitConv(51).mult = 2118.6438;
        ort->UnitConv(52).mult = 15852.0;
        ort->UnitConv(53).mult = 196.85;
        ort->UnitConv(54).mult = 2118.6438;
        ort->UnitConv(55).mult = 15852.0;
        ort->UnitConv(56).mult = 0.0001450377;
        ort->UnitConv(57).mult = 0.00029613;
        ort->UnitConv(58).mult = 0.00401463;
        ort->UnitConv(59).mult = 0.00033455;
        ort->UnitConv(60).mult = 0.0001450377;
        ort->UnitConv(61).mult = 0.00029613;
        ort->UnitConv(62).mult = 0.00401463;
        ort->UnitConv(63).mult = 0.00033455;
        ort->UnitConv(64).mult = 1.0;
        ort->UnitConv(65).mult = 1.0;
        ort->UnitConv(66).mult = 3.412;
        ort->UnitConv(67).mult = 1.0;
        ort->UnitConv(68).mult = 0.001;
        ort->UnitConv(69).mult = 0.00341442;
        ort->UnitConv(70).mult = 0.0002843333;
        ort->UnitConv(71).mult = 0.001547673;
        ort->UnitConv(72).mult = 0.001547673;
        ort->UnitConv(73).mult = 1.8987;
        ort->UnitConv(74).mult = 0.316954237;
        ort->UnitConv(75).mult = 0.000316954237;
        ort->UnitConv(76).mult = 0.176085687;
        ort->UnitConv(77).mult = 0.176085687;
        ort->UnitConv(78).mult = 1.0;
        ort->UnitConv(79).mult = 1.0;
        ort->UnitConv(80).mult = 1.8;
        ort->UnitConv(81).mult = 0.42956;
        ort->UnitConv(82).mult = 1.0 / 2118.6438;
        ort->UnitConv(83).mult = 1.0 / 15852;
        ort->UnitConv(84).mult = 1.0 / 10.764961;
        ort->UnitConv(85).mult = 0.00000094845 * 1000000000;
        ort->UnitConv(86).mult = 0.000000277778 * 1000000000;
        ort->UnitConv(87).mult = 0.000000277778 * 1000000000;
        ort->UnitConv(88).mult = 0.0000000094845 * 1000000000;
        ort->UnitConv(89).mult = 0.00000000094845 * 1000000000;
        ort->UnitConv(90).mult = 0.000277777777777778 * 1000000000;
        ort->UnitConv(91).mult = 0.0000000789847 * 1000000000;
        ort->UnitConv(92).mult = 0.277777777777778 / 10.764961;
        ort->UnitConv(93).mult = 0.94708628903179 / 10.764961;
        ort->UnitConv(94).mult = 0.94708628903179 / 10.764961;
        ort->UnitConv(95).mult = 0.27777777777778;
        ort->UnitConv(96).mult = 1.0;
        ort->UnitConv(97).mult = 1.0;
        ort->UnitConv(98).mult = 1.8987;
        ort->UnitConv(99).mult = 1.0;
        ort->UnitConv(100).mult = 1.0;
        ort->UnitConv(101).mult = 1.0;
        ort->UnitConv(102).mult = 1.0;
        ort->UnitConv(103).mult = 1.0;
        ort->UnitConv(104).mult = 1.0;
        ort->UnitConv(105).mult = 1.0;
        ort->UnitConv(106).mult = 1.0;
        ort->UnitConv(107).mult = 1.0;
        ort->UnitConv(108).mult = 0.000238845896627;
        ort->UnitConv(109).mult = 2.2046;
        ort->UnitConv(110).mult = 1.0;
        ort->UnitConv(111).mult = 1.0;
        ort->UnitConv(112).mult = 1.0;
        ort->UnitConv(113).mult = 0.737562149277;
        ort->UnitConv(114).mult = 1.0;
        ort->UnitConv(115).mult = 0.09290304;

        ort->UnitConv(2).offset = 32.0;
        ort->UnitConv(11).offset = 32.0;
        ort->UnitConv(25).offset = 7.6736;
        ort->UnitConv(81).offset = 7.6736; // 80 is KJ/KG -- should this be multiplied by 1000?

        ort->UnitConv(20).hint = "ELEC";
        ort->UnitConv(21).hint = "GAS";
        ort->UnitConv(24).hint = "COOL";
        ort->UnitConv(38).hint = "WATER";
        ort->UnitConv(49).hint = "WATER";
        ort->UnitConv(52).hint = "WATER";
        ort->UnitConv(67).hint = "ELEC";
        ort->UnitConv(70).hint = "COOL";
        ort->UnitConv(79).hint = "SI";
        ort->UnitConv(83).hint = "WATER";
        ort->UnitConv(86).hint = "CONSUMP";
        ort->UnitConv(87).hint = "ELEC";
        ort->UnitConv(88).hint = "GAS";
        ort->UnitConv(91).hint = "COOL";
        ort->UnitConv(92).hint = "ELEC";
        ort->UnitConv(93).hint = "GAS";
        ort->UnitConv(93).hint = "ADDITIONAL";

        ort->UnitConv(19).several = true;
        ort->UnitConv(20).several = true;
        ort->UnitConv(21).several = true;
        ort->UnitConv(22).several = true;
        ort->UnitConv(23).several = true;
        ort->UnitConv(24).several = true;
        ort->UnitConv(38).several = true;
        ort->UnitConv(39).several = true;
        ort->UnitConv(42).several = true;
        ort->UnitConv(43).several = true;
        ort->UnitConv(44).several = true;
        ort->UnitConv(45).several = true;
        ort->UnitConv(48).several = true;
        ort->UnitConv(49).several = true;
        ort->UnitConv(51).several = true;
        ort->UnitConv(52).several = true;
        ort->UnitConv(54).several = true;
        ort->UnitConv(55).several = true;
        ort->UnitConv(56).several = true;
        ort->UnitConv(57).several = true;
        ort->UnitConv(58).several = true;
        ort->UnitConv(59).several = true;
        ort->UnitConv(60).several = true;
        ort->UnitConv(61).several = true;
        ort->UnitConv(62).several = true;
        ort->UnitConv(63).several = true;
        ort->UnitConv(66).several = true;
        ort->UnitConv(67).several = true;
        ort->UnitConv(68).several = true;
        ort->UnitConv(69).several = true;
        ort->UnitConv(70).several = true;
        ort->UnitConv(74).several = true;
        ort->UnitConv(75).several = true;
        ort->UnitConv(78).several = true;
        ort->UnitConv(79).several = true;
        ort->UnitConv(82).several = true;
        ort->UnitConv(83).several = true;
        ort->UnitConv(85).several = true;
        ort->UnitConv(86).several = true;
        ort->UnitConv(87).several = true;
        ort->UnitConv(88).several = true;
        ort->UnitConv(89).several = true;
        ort->UnitConv(90).several = true;
        ort->UnitConv(91).several = true;
        ort->UnitConv(92).several = true;
        ort->UnitConv(93).several = true;
        ort->UnitConv(94).several = true;
        ort->UnitConv(95).several = true;
    }

    std::string GetUnitSubString(std::string const &inString) // Input String
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Jason Glazer
        //       DATE WRITTEN   February 2013
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   return the substring contained in brackets []
        //   that contains the units.

        // METHODOLOGY EMPLOYED:
        //   na

        // Return value
        std::string outUnit; // Result String

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        // na

        // INTERFACE BLOCK SPECIFICATIONS:
        // na

        // DERIVED TYPE DEFINITIONS:
        // na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:

        // check if string has brackets or parentheses
        std::string::size_type const posLBrac = index(inString, '['); // left bracket
        std::string::size_type const posRBrac = index(inString, ']'); // right bracket
        // extract the substring with the units
        if ((posLBrac != std::string::npos) && (posRBrac != std::string::npos) && (posRBrac - posLBrac >= 1)) {
            outUnit = inString.substr(posLBrac + 1, posRBrac - posLBrac - 1);
        }
        return outUnit;
    }

    void LookupSItoIP(EnergyPlusData &state, std::string const &stringInWithSI, int &unitConvIndex, std::string &stringOutWithIP)
    {
        // SUBROUTINE INFORMATION:
        //    AUTHOR         Jason Glazer of GARD Analytics, Inc.
        //    DATE WRITTEN   February 12, 2009

        // PURPOSE OF THIS SUBROUTINE:
        //   The input string to this subroutine can either contain
        //   a unit that should be looked up or it can contain
        //   but the unit and other text (such as the column heading)
        //   that includes a hint for when the unit may have multiple
        //   possible conversions. If the input string includes
        //   just the unit it does not have either brackets or
        //   parenthesis. If the string includes text with a possible
        //   hint the units themselves will be in either brackets
        //   or parentheses. The index to the unitConv array is returned
        //   which can be used with the convertIP function. Also the
        //   string with the IP units substituted is returned.

        std::string unitSIOnly;
        int modeInString;
        int const misBrac(1);
        int const misParen(2);
        int const misBrce(3);
        int const misNoHint(4);
        std::string const stringInUpper(UtilityRoutines::MakeUPPERCase(stringInWithSI));
        auto &ort(state.dataOutRptTab);

        stringOutWithIP = "";
        // check if string has brackets or parentheses
        std::string::size_type posLBrac = index(stringInUpper, '[');  // left bracket
        std::string::size_type posRBrac = index(stringInUpper, ']');  // right bracket
        std::string::size_type posLParen = index(stringInUpper, '('); // left parenthesis
        std::string::size_type posRParen = index(stringInUpper, ')'); // right parenthesis
        std::string::size_type posLBrce = index(stringInUpper, '{');  // left brace
        std::string::size_type posRBrce = index(stringInUpper, '}');  // right brace
        bool noBrackets = true;
        // extract the substring with the units
        if ((posLBrac != std::string::npos) && (posRBrac != std::string::npos) && (posRBrac - posLBrac >= 1)) {
            unitSIOnly = stringInUpper.substr(posLBrac + 1, posRBrac - posLBrac - 1);
            modeInString = misBrac;
            noBrackets = false;
        } else if ((posLBrce != std::string::npos) && (posRBrce != std::string::npos) && (posRBrce - posLBrce >= 1)) {
            unitSIOnly = stringInUpper.substr(posLBrce + 1, posRBrce - posLBrce - 1);
            modeInString = misBrce;
        } else if ((posLParen != std::string::npos) && (posRParen != std::string::npos) && (posRParen - posLParen >= 1)) {
            unitSIOnly = stringInUpper.substr(posLParen + 1, posRParen - posLParen - 1);
            modeInString = misParen;
        } else {
            unitSIOnly = stringInUpper;
            modeInString = misNoHint;
        }
        unitSIOnly = stripped(unitSIOnly);
        int defaultConv = 0;
        int foundConv = 0;
        int firstOfSeveral = 0;
        for (int iUnit = 1; iUnit <= ort->UnitConvSize; ++iUnit) {
            if (UtilityRoutines::SameString(ort->UnitConv(iUnit).siName, unitSIOnly)) {
                if (ort->UnitConv(iUnit).several) {
                    if (firstOfSeveral == 0) firstOfSeveral = iUnit;
                    if (ort->UnitConv(iUnit).is_default) defaultConv = iUnit;
                    // look for the hint string
                    if (len(ort->UnitConv(iUnit).hint) > 0) {
                        if (has(stringInUpper, ort->UnitConv(iUnit).hint)) {
                            foundConv = iUnit;
                            break;
                        }
                    }
                } else { // not several possibilities so don't bother with rest of array
                    foundConv = iUnit;
                    break;
                }
            }
        }
        // if it is found set the selected value to what was found. if not found,
        // directly set it to the default and if no default set it to the first item
        // in group.  Return zero if not found.
        int selectedConv(0);
        if (foundConv > 0) {
            selectedConv = foundConv;
        } else {
            // not found - see if in a group it should be default or first.
            if (firstOfSeveral == 0) {
                selectedConv = 0;
            } else {
                if (defaultConv != 0) {
                    selectedConv = defaultConv;
                } else {
                    selectedConv = firstOfSeveral;
                }
            }
        }
        // if one was selected substitute the units into the output string
        if (selectedConv > 0) {
            if (modeInString == misBrac) {
                stringOutWithIP = stringInWithSI.substr(0, posLBrac + 1) + ort->UnitConv(selectedConv).ipName + stringInWithSI.substr(posRBrac);
            } else if (modeInString == misParen) {
                stringOutWithIP = stringInWithSI.substr(0, posLParen + 1) + ort->UnitConv(selectedConv).ipName + stringInWithSI.substr(posRParen);
            } else if (modeInString == misBrce) {
                stringOutWithIP = stringInWithSI.substr(0, posLBrce + 1) + ort->UnitConv(selectedConv).ipName + stringInWithSI.substr(posRBrce);
            } else if (modeInString == misNoHint) {
                stringOutWithIP = ort->UnitConv(selectedConv).ipName;
            }
        } else {
            // if no conversion just output the input string
            stringOutWithIP = stringInWithSI;
        }
        // For debugging only
        // CALL  ShowWarningError(state, 'LookupSItoIP in: ' // TRIM(stringInWithSI) // ' out: ' // TRIM(stringOutWithIP))
        // IF (foundConv .NE. 0) CALL  ShowWarningError(state, '   Hint ' // TRIM(UnitConv(foundConv)%hint) // std::to_string(foundConv) )

        unitConvIndex = selectedConv;

        // Add warning if units not found.
        if (unitConvIndex == 0 && !noBrackets) {
            ShowWarningError(state, "Unable to find a unit conversion from " + stringInWithSI + " into IP units");
            ShowContinueError(state, "Applying default conversion factor of 1.0");
        }
    }

    void LookupJtokWH(EnergyPlusData &state, std::string const &stringInWithJ, int &unitConvIndex, std::string &stringOutWithKWH)
    {
        //    AUTHOR         Jason Glazer of GARD Analytics, Inc.
        //    DATE WRITTEN   April 15, 2016

        // For the given unit expressed in J find the unit conversion
        // using kWh instead. This is used when unitsStyle == unitsStyleJtoKWH
        // return zero if no unit conversion should be done

        stringOutWithKWH = stringInWithJ;
        std::string::size_type gjPos = stringOutWithKWH.find("[GJ]");
        std::string::size_type mjm2Pos = stringOutWithKWH.find("[MJ/m2]");
        if (gjPos != std::string::npos) {
            stringOutWithKWH.replace(gjPos, 4, "[kWh]");
            unitConvIndex = getSpecificUnitIndex(state, "GJ", "kWh");
        } else if (mjm2Pos != std::string::npos) {
            stringOutWithKWH.replace(mjm2Pos, 7, "[kWh/m2]");
            unitConvIndex = getSpecificUnitIndex(state, "MJ/m2", "kWh/m2");
        } else {
            unitConvIndex = 0;
        }
    }

    Real64 ConvertIP(EnergyPlusData &state, int const unitConvIndex, Real64 const SIvalue)
    {
        // SUBROUTINE INFORMATION:
        //    AUTHOR         Jason Glazer of GARD Analytics, Inc.
        //    DATE WRITTEN   February 13, 2009
        //    MODIFIED       September 2012
        //    RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Apply the selected unit conversion to the input value
        //   expressed in SI units to result in IP units.
        //   If zero is provided as unit index, return the original
        //   value (no conversion)

        // METHODOLOGY EMPLOYED:

        // REFERENCES:
        //    na

        // USE STATEMENTS:

        // Return value
        Real64 ConvertIP;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        //    na

        // INTERFACE BLOCK SPECIFICATIONS:
        //    na

        // DERIVED TYPE DEFINITIONS:
        //    na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        //    na

        auto &ort(state.dataOutRptTab);

        if (unitConvIndex == 0 || SIvalue == -999.0 || SIvalue == -99999.0) { // don't convert unknown data to IP
            ConvertIP = SIvalue;
        } else if ((unitConvIndex > 0) && (unitConvIndex <= ort->UnitConvSize)) {
            ConvertIP = (SIvalue * ort->UnitConv(unitConvIndex).mult) + ort->UnitConv(unitConvIndex).offset;
        } else {
            ConvertIP = SIvalue;
        }
        return ConvertIP;
    }

    Real64 ConvertIPdelta(EnergyPlusData &state, int const unitConvIndex, Real64 const SIvalue)
    {
        // SUBROUTINE INFORMATION:
        //    AUTHOR         Jason Glazer of GARD Analytics, Inc.
        //    DATE WRITTEN   February 18, 2009
        //    MODIFIED       na
        //    RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Apply the selected unit conversion to the input value
        //   expressed in SI units to result in IP units. This routine
        //   only uses the mulitplier and NOT the offset and is appropriate
        //   when the number being converted is a difference or delta
        //   between values (such as a temperature difference).

        // METHODOLOGY EMPLOYED:

        // REFERENCES:
        //    na

        // USE STATEMENTS:

        // Return value
        Real64 ConvertIPdelta;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        //    na

        // INTERFACE BLOCK SPECIFICATIONS:
        //    na

        // DERIVED TYPE DEFINITIONS:
        //    na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        //    na

        auto &ort(state.dataOutRptTab);

        if (unitConvIndex == 0) {
            ConvertIPdelta = SIvalue;
        } else if ((unitConvIndex > 0) && (unitConvIndex <= ort->UnitConvSize)) {
            ConvertIPdelta = SIvalue * ort->UnitConv(unitConvIndex).mult;
        } else {
            ConvertIPdelta = SIvalue;
        }
        return ConvertIPdelta;
    }

    void GetUnitConversion(EnergyPlusData &state, int const unitConvIndex, Real64 &multiplier, Real64 &offset, std::string &IPunit)
    {
        // SUBROUTINE INFORMATION:
        //    AUTHOR         Jason Glazer of GARD Analytics, Inc.
        //    DATE WRITTEN   February 13, 2009
        //    MODIFIED       na
        //    RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Return of the multiplier and adder for the given
        //   SI to IP unit conversion.
        //     SI = (IP * multipier) + offset
        //  This function could be replaced by referencing the
        //  array directly but does include some checking of the
        //  bounds of the array.

        // METHODOLOGY EMPLOYED:

        // REFERENCES:
        //    na

        // USE STATEMENTS:

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        //    na

        // INTERFACE BLOCK SPECIFICATIONS:
        //    na

        // DERIVED TYPE DEFINITIONS:
        //    na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        //    na

        auto &ort(state.dataOutRptTab);

        if ((unitConvIndex > 0) && (unitConvIndex <= ort->UnitConvSize)) {
            multiplier = ort->UnitConv(unitConvIndex).mult;
            offset = ort->UnitConv(unitConvIndex).offset;
            IPunit = ort->UnitConv(unitConvIndex).ipName;
        } else {
            multiplier = 1.0;
            offset = 0.0;
            IPunit = "";
        }
    }

    Real64 getSpecificUnitMultiplier(EnergyPlusData &state, std::string const &SIunit, std::string const &IPunit)
    {
        // SUBROUTINE INFORMATION:
        //    AUTHOR         Jason Glazer of GARD Analytics, Inc.
        //    DATE WRITTEN   February 13, 2009
        //    MODIFIED       na
        //    RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Return of the multiplier for a specific
        //   SI to IP unit conversion. No offset is provided so
        //   it cannot be used to convert units such as temperatures
        //   that include an offset.
        //     SI = (IP * multipier) + offset
        //   Unlike LookupSItoIP, this function does not expect more
        //   the units in the two input parameters. No hints or
        //   defaults are used since both the SI and IP units are
        //   input by the user.

        // METHODOLOGY EMPLOYED:

        // REFERENCES:
        //    na

        // USE STATEMENTS:

        // Return value
        Real64 getSpecificUnitMultiplier;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        //    na

        // INTERFACE BLOCK SPECIFICATIONS:
        //    na

        // DERIVED TYPE DEFINITIONS:
        //    na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        static int found(0);
        static int iUnit(0);
        auto &ort(state.dataOutRptTab);

        for (iUnit = 1; iUnit <= ort->UnitConvSize; ++iUnit) {
            if (UtilityRoutines::SameString(ort->UnitConv(iUnit).siName, SIunit)) {
                if (UtilityRoutines::SameString(ort->UnitConv(iUnit).ipName, IPunit)) {
                    found = iUnit;
                    break;
                }
            }
        }
        if (found != 0) {
            getSpecificUnitMultiplier = ort->UnitConv(found).mult;
        } else {
            ShowWarningError(state, "Unable to find a unit conversion from " + SIunit + " to " + IPunit);
            ShowContinueError(state, "Applying default conversion factor of 1.0");
            getSpecificUnitMultiplier = 1.0;
        }
        return getSpecificUnitMultiplier;
    }

    Real64 getSpecificUnitDivider(EnergyPlusData &state, std::string const &SIunit, std::string const &IPunit)
    {
        // SUBROUTINE INFORMATION:
        //    AUTHOR         Jason Glazer of GARD Analytics, Inc.
        //    DATE WRITTEN   February 13, 2009
        //    MODIFIED       na
        //    RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Returns the divider (1/multiplier) for a specific
        //   SI to IP unit conversion. No offset is provided so
        //   it cannot be used to convert units such as temperatures
        //   that include an offset.
        //     SI = (IP * multipier) + offset
        //   Unlike LookupSItoIP, this function does not expect more
        //   the units in the two input parameters. No hints or
        //   defaults are used since both the SI and IP units are
        //   input by the user.

        // METHODOLOGY EMPLOYED:

        // REFERENCES:
        //    na

        // USE STATEMENTS:

        // Return value
        Real64 getSpecificUnitDivider;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        //    na

        // INTERFACE BLOCK SPECIFICATIONS:
        //    na

        // DERIVED TYPE DEFINITIONS:
        //    na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        Real64 mult;

        mult = getSpecificUnitMultiplier(state, SIunit, IPunit);
        if (mult != 0) {
            getSpecificUnitDivider = 1 / mult;
        } else {
            ShowWarningError(state, "Unable to find a unit conversion from " + SIunit + " to " + IPunit);
            ShowContinueError(state, "Applying default conversion factor of 1.0");
            getSpecificUnitDivider = 1.0;
        }
        return getSpecificUnitDivider;
    }

    Real64 getSpecificUnitIndex(EnergyPlusData &state, std::string const &SIunit, std::string const &IPunit)
    {
        // SUBROUTINE INFORMATION:
        //    AUTHOR         Jason Glazer of GARD Analytics, Inc.
        //    DATE WRITTEN   September 21, 2012
        //    MODIFIED       na
        //    RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        //   Return of the unit conversion index for a specific
        //   SI to IP unit conversion.
        //   Unlike LookupSItoIP, this function does not expect more
        //   the units in the two input parameters. No hints or
        //   defaults are used since both the SI and IP units are
        //   input by the user.

        // METHODOLOGY EMPLOYED:

        // REFERENCES:
        //    na

        // USE STATEMENTS:

        // Return value
        Real64 getSpecificUnitIndex;

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE PARAMETER DEFINITIONS:
        //    na

        // INTERFACE BLOCK SPECIFICATIONS:
        //    na

        // DERIVED TYPE DEFINITIONS:
        //    na

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        static int found(0);
        static int iUnit(0);
        auto &ort(state.dataOutRptTab);

        for (iUnit = 1; iUnit <= ort->UnitConvSize; ++iUnit) {
            if (UtilityRoutines::SameString(ort->UnitConv(iUnit).siName, SIunit)) {
                if (UtilityRoutines::SameString(ort->UnitConv(iUnit).ipName, IPunit)) {
                    found = iUnit;
                    break;
                }
            }
        }
        if (found != 0) {
            getSpecificUnitIndex = found;
        } else {
            getSpecificUnitIndex = 0.0;
        }
        return getSpecificUnitIndex;
    }

} // namespace EnergyPlus
