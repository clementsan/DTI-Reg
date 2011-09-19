#include <iostream>
#include <fstream>

#include <itksys/SystemTools.hxx>

#include <bmScriptParser.h>

#include "DTI-RegCLP.h"
#include "DTI-Reg_Config.h"


int SetPath( std::string &pathString , const char* name , std::vector< std::string >  path_vec )
{
  if( pathString.empty() || !pathString.substr(pathString.size() - 10 , 9 ).compare( "-NOTFOUND" ) )
  {
    pathString= itksys::SystemTools::FindProgram( name , path_vec ) ;
    if( !pathString.compare( "" ) )
    {
      std::cerr << name << " is missing or its PATH is not set" << std::endl ;
      return -1 ;
    }
  }
  return 0 ;
}

int main (int argc, char *argv[])
{
  PARSE_ARGS;
  
  std::vector< std::string > path_vec ;

  std::cout<<"DTI-Reg..."<<std::endl;

  if( fixedVolume.empty() || movingVolume.empty() )
  {
    std::cerr << "Error: Fixed and moving volumes must be set!" << std::endl ;
    std::cerr << "To display help, type: DTI-Reg --help"<<std::endl;
    return EXIT_FAILURE ;
  }  
  if (!method.compare("useTensor"))
    {
      std::cout<<"Computational method not implemented yet..."<<std::endl;
      return EXIT_FAILURE;
    }
  if (!linearRegType.compare("None") && !warpingType.compare("None"))
    {
      std::cout<<"No registration to be performed..."<<std::endl;
      return EXIT_FAILURE;
    }

  // Write BatchMake script
  std::string BatchMakeScriptFile = "DTI-Reg.bms";
  std::ofstream file( BatchMakeScriptFile.c_str());

  file <<"# I/O Parameters"<<std::endl;
  file <<"set (fixedVolume "<<fixedVolume<<")"<<std::endl;
  file <<"set (movingVolume "<<movingVolume<<")"<<std::endl;
  file <<"set (outputVolume "<<outputVolume<<")"<<std::endl;

  file <<"# Registration parameters"<<std::endl;
  file <<"set (linearRegType "<<linearRegType<<")"<<std::endl;
  if (!linearRegType.compare("None"))
    file <<"set (IsLinearRegistration 0)"<<std::endl;
  else
    file <<"set (IsLinearRegistration 1)"<<std::endl;

  file <<"set (warpingType "<<warpingType<<")"<<std::endl;
  if (!warpingType.compare("None") || !warpingType.compare("BSpline"))
    file <<"set (IsDemonsWarping 0)"<<std::endl;
  else
    file <<"set (IsDemonsWarping 1)"<<std::endl;
  
  file <<"# Advanced registration parameters"<<std::endl;
  
  if (fixedMaskVolume.compare(""))
    file <<"set (fixedMaskVolume "<<fixedMaskVolume<<")"<<std::endl;
  else
    file <<"set (fixedMaskVolume \'\')"<<std::endl;
  if (movingMaskVolume.compare(""))
    file <<"set (movingMaskVolume "<<movingMaskVolume<<")"<<std::endl;
  else
    file <<"set (movingMaskVolume \'\')"<<std::endl;

  file <<"# Advanced Histogram matching parameters"<<std::endl;
  file <<"set (numberOfHistogramLevels "<<numberOfHistogramLevels<<")"<<std::endl;
  file <<"set (numberOfMatchPoints "<<numberOfMatchPoints<<")"<<std::endl;

  file <<"#External Tools"<<std::endl;
  std::string dtiprocessCmd = dtiprocessTool;
  if( SetPath(dtiprocessCmd, "dtiprocess" , path_vec ) )
    return EXIT_FAILURE;
  else
    file <<"set (dtiprocessCmd "<<dtiprocessCmd<<")"<<std::endl;
  std::string HistogramMatchingCmd = HistogramMatchingTool;
  if( SetPath(HistogramMatchingCmd, "HistogramMatching" , path_vec ) )
    return EXIT_FAILURE;
  else
    file <<"set (HistogramMatchingCmd "<<HistogramMatchingCmd<<")"<<std::endl;
  std::string BRAINSFitCmd = BRAINSFitTool;
  if( SetPath(BRAINSFitCmd, "BRAINSFit" , path_vec ) )
    return EXIT_FAILURE;
  else
    file <<"set (BRAINSFitCmd "<<BRAINSFitCmd<<")"<<std::endl;
  std::string BRAINSDemonWarpCmd = BRAINSDemonWarpTool;
  if( SetPath(BRAINSDemonWarpCmd, "BRAINSDemonWarp" , path_vec ) )
    return EXIT_FAILURE;
  else
    file <<"set (BRAINSDemonWarpCmd "<<BRAINSDemonWarpCmd<<")"<<std::endl;
  std::string ResampleDTICmd = ResampleDTITool;
  if( SetPath(ResampleDTICmd, "ResampleDTI" , path_vec ) )
    return EXIT_FAILURE;
  else
    file <<"set (ResampleDTICmd "<<ResampleDTICmd<<")"<<std::endl;

  // Include main BatchMake script
  file <<"include("<<ScriptDir<<"/DTI-Reg_Scalar.bms)"<<std::endl;

  file.close();


  // Execute BatchMake
  bm::ScriptParser m_Parser;
  m_Parser.Execute(BatchMakeScriptFile);

  std::cout<<"DTI-Reg: Done!"<<std::endl;
  return EXIT_SUCCESS;
}
