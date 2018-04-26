// SMCodeGen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <Shlwapi.h>
#include "ShapeParser.h"
#include "CodeGen.h"

#define NEW_LINE                                    "\r\n"

int ParseAndGen(const std::vector<std::string> &arrShapeXmlFileContent, bool bDump, std::vector<std::string> &arrDumpContent, 
                const std::string &sCodeOutputDir, const std::string &sCodePrifex)
{
    int nError = -1;
    SP_ErrorCode nSpError;
    CG_ErrorCode nCgError;
    CodeGen *pCodeGen = NULL;
    unsigned int i, j, k;
    std::vector<ShapeParser *> arrShapeParsers;
    std::vector<ShapeState *> arrShapeStates;
    std::vector<ShapeTrans *> arrShapeTranss;

    arrDumpContent.clear();
    for (i = 0; i < arrShapeXmlFileContent.size(); i++) {
        const std::string &oShapeXmlFileContent = arrShapeXmlFileContent[i];

        ShapeParser *pShapeParser = new ShapeParser(oShapeXmlFileContent);
        if (pShapeParser == NULL) {
            nError = -1;
            goto ERR1;
        }
        arrShapeParsers.push_back(pShapeParser);

        nSpError = pShapeParser->DoParsing();
        if (nSpError != SP_EC_OK) {
            TK_ERR("*** ERROR[ShapeParser]: [code=%s]:\n%s\n", 
                pShapeParser->GetErrorStr(nSpError).c_str(), 
                pShapeParser->GetLastErrorXml().c_str()
            );
            nError = -1;
            goto ERR1;
        }

        for (j = 0; j < pShapeParser->GetShapeStates().size(); j++) {
            ShapeState *pShapeState_Sub = &pShapeParser->GetShapeStates()[j];
            if (pShapeState_Sub->pParentShapeState == NULL) {
                for (k = 0; k < arrShapeStates.size(); k++) {
                    ShapeState *pShapeState = arrShapeStates[k];
                    if (pShapeState_Sub->sStateName == pShapeState->sStateName) {
                        //
                        // for bool ShapeState::bIsRootStateInSubDiagram;
                        //
                        pShapeState_Sub->bIsRootStateInSubDiagram = true;

                        //
                        // for std::vector<TransItem> ShapeState::arrInternalTransItems;
                        //
                        pShapeState->arrInternalTransItems.insert(pShapeState->arrInternalTransItems.end(), 
                            pShapeState_Sub->arrInternalTransItems.begin(), pShapeState_Sub->arrInternalTransItems.end());
                        pShapeState_Sub->arrInternalTransItems.clear();
                        pShapeState_Sub->arrInternalTransItems.insert(pShapeState_Sub->arrInternalTransItems.end(), 
                            pShapeState->arrInternalTransItems.begin(), pShapeState->arrInternalTransItems.end());

                        //
                        // for ShapeState *ShapeState::pParentShapeState;
                        //
                        pShapeState_Sub->pParentShapeState = pShapeState->pParentShapeState;

                        //
                        // for std::vector<ShapeState *> ShapeState::arrChildShapeStates;
                        //
                        pShapeState->arrChildShapeStates.insert(pShapeState->arrChildShapeStates.end(),
                            pShapeState_Sub->arrChildShapeStates.begin(), pShapeState_Sub->arrChildShapeStates.end());
                        pShapeState_Sub->arrChildShapeStates.clear();
                        pShapeState_Sub->arrChildShapeStates.insert(pShapeState_Sub->arrChildShapeStates.end(), 
                            pShapeState->arrChildShapeStates.begin(), pShapeState->arrChildShapeStates.end());

                        //
                        // for std::vector<ShapeTrans *> ShapeState::arrShapeTransFromMe;
                        //
                        pShapeState->arrShapeTransFromMe.insert(pShapeState->arrShapeTransFromMe.end(),
                            pShapeState_Sub->arrShapeTransFromMe.begin(), pShapeState_Sub->arrShapeTransFromMe.end());
                        pShapeState_Sub->arrShapeTransFromMe.clear();
                        pShapeState_Sub->arrShapeTransFromMe.insert(pShapeState_Sub->arrShapeTransFromMe.end(), 
                            pShapeState->arrShapeTransFromMe.begin(), pShapeState->arrShapeTransFromMe.end());

                        //
                        // for std::vector<ShapeTrans *> ShapeState::arrShapeTransToMe;
                        //
                        pShapeState->arrShapeTransToMe.insert(pShapeState->arrShapeTransToMe.end(),
                            pShapeState_Sub->arrShapeTransToMe.begin(), pShapeState_Sub->arrShapeTransToMe.end());
                        pShapeState_Sub->arrShapeTransToMe.clear();
                        pShapeState_Sub->arrShapeTransToMe.insert(pShapeState_Sub->arrShapeTransToMe.end(), 
                            pShapeState->arrShapeTransToMe.begin(), pShapeState->arrShapeTransToMe.end());

                        break;
                    } // if (pShapeState_Sub->sStateName == pShapeState->sStateName) {
                } // for (k = 0; k < arrShapeStates.size(); k++) {
                if (k >= arrShapeStates.size()) {
                    arrShapeStates.push_back(pShapeState_Sub);
                }
            } else { // if (pShapeState_Sub->pParentShapeState == NULL) {
                arrShapeStates.push_back(pShapeState_Sub);
            }
        } // for (j = 0; j < pShapeParser->GetShapeStates().size(); j++) {

        for (j = 0; j < pShapeParser->GetShapeTranss().size(); j++) {
            ShapeTrans *pShapeTrans = &pShapeParser->GetShapeTranss()[i];
            arrShapeTranss.push_back(pShapeTrans);
        }

        if (bDump) {
            std::string sDumpContent;
            nSpError = pShapeParser->Dump(sDumpContent);
            if (nSpError != SP_EC_OK) {
                TK_ERR("*** ERROR[ShapeParser]: [code=%s]:\n%s\n", 
                    pShapeParser->GetErrorStr(nSpError).c_str(), 
                    pShapeParser->GetLastErrorXml().c_str()
                );
                nError = -1;
                goto ERR1;
            }
            arrDumpContent.push_back(sDumpContent);
        }
    } // for (i = 0; i < arrShapeXmlFileContent.size(); i++) {

    pCodeGen = new CodeGen(arrShapeStates, arrShapeTranss);
    if (pCodeGen == NULL) {
        nError = -1;
        goto ERR1;
    }
    pCodeGen->SetOutputDir(sCodeOutputDir);
    pCodeGen->SetPrefix(sCodePrifex);
    nCgError = pCodeGen->Generate();
    if (nCgError != CG_EC_OK) {
        TK_ERR("*** ERROR[CodeGen]: [code=%s]:\n", 
            pCodeGen->GetErrorStr(nCgError).c_str()
        );
        nError = -1;
        goto ERR1;
    }
    
    nError = 0;
ERR1:
    if (pCodeGen != NULL) {
        delete pCodeGen;
        pCodeGen = NULL;
    }
    for (i = 0; i < arrShapeParsers.size(); i++) {
        delete arrShapeParsers[i];
    }
    return nError;
}

void usage(int argc, char* argv[])
{
    TK_MSG("Usage:\n");
    TK_MSG("  %s <code_prefix> <gen_src_dir> <dump_xml_path>|not_dump <shapes_xml_path> [<shapes_xml_path>] ...\n", argv[0]);
    TK_MSG("\n");
}

int main(int argc, char* argv[])
{
    int nRet = -1;
    bool bSuc;
    std::string sCodePrifex;
    std::string sCodeOutputDir;
    std::string sDumpXmlPath;
    std::vector<std::string> arrInputFilePath;
    std::vector<std::string> arrFileContent;
    std::vector<std::string> arrDumpContent;
    bool bDump = false;
    unsigned int i;

    CoInitialize(NULL);

    if (argc < 5) {
        usage(argc, argv);
        return 0;
    }

    sCodePrifex = argv[1];
    sCodeOutputDir = argv[2];
    sDumpXmlPath = argv[3];
    arrInputFilePath.push_back(argv[4]);
    
    for (i = 5; i < (unsigned int)argc; i++) {
        arrInputFilePath.push_back(argv[i]);
    }

    if (!PathIsDirectory(TK_Tools::str2wstr(sCodeOutputDir).c_str())) {
        TK_ERR("*** ERROR: can't write to dir \"%s\"\n", 
            sCodeOutputDir.c_str()
        );
        nRet = -1;
        goto ERR1;
    }

    if (sDumpXmlPath != "not_dump") {
        bDump = true;
    }

    for (i = 0; i < arrInputFilePath.size(); i++) {
        std::string sFileContent;
        bSuc = TK_Tools::LoadFromFile(arrInputFilePath[i], sFileContent);
        if (!bSuc) {
            nRet = -1;
            goto ERR1;
        }
        arrFileContent.push_back(sFileContent);
    }

    nRet = ParseAndGen(arrFileContent, bDump, arrDumpContent, sCodeOutputDir, sCodePrifex);
    if (nRet != 0) {
        nRet = -1;
        goto ERR1;
    }

    if (bDump) {
        std::string sDumpContent;
        for (i = 0; i < arrDumpContent.size(); i++) {
            sDumpContent +=
                TK_Tools::FormatStr("<!-- The end of the file %d. -->%s%s", i + 1, NEW_LINE, NEW_LINE)
                + arrDumpContent[i];
        }

        bSuc = TK_Tools::SaveToFile(sDumpXmlPath, sDumpContent);
        if (!bSuc) {
            nRet = -1;
            goto ERR1;
        }
    }

    nRet = 0;
ERR1:
    CoUninitialize();
	return nRet;
}

