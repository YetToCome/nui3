/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "Generator.h"
#include "Application.h"
#include "nuiVBox.h"
#include "nuiHBox.h"
#include "nuiDialogSelectDirectory.h"
#include "nuiDialogSelectFile.h"
#include "nuiMessageBox.h"
#include "nuiColorDecoration.h"
#include "MainWindow.h"



Generator::Generator()
: nuiSimpleContainer(), mEventSink(this), mTimerSink(this), mTimer(0.5)
{
  // deco
  SetBorder(20,20);
  
  nuiColorDecoration* pDeco = new nuiColorDecoration(_T("MyDeco"), nuiRect(0,0,0,0), nuiColor(200,200,200), 1, nuiColor(180,180,180), eStrokeAndFillShape);
  //*****************************************
  
  
  
  nuiVBox* pMainBox = new nuiVBox(0);
  pMainBox->SetExpand(nuiExpandShrinkAndGrow);
  AddChild(pMainBox);
  
  
  //*******************************************************************
  // bloc for generator tool
  //
  nuiVBox* pBloc = new nuiVBox(0);
  pBloc->SetExpand(nuiExpandShrinkAndGrow);
  pMainBox->AddCell(pBloc);
  
  pBloc->AddCell(new nuiLabel(_T("generator command line tool:")));
  pBloc->SetCellExpand(pBloc->GetNbCells()-1, nuiExpandShrinkAndGrow);  

  nuiHBox* pBox = new nuiHBox(2);
  pBloc->AddCell(pBox);
  
  nglPath prefPath;
  GetApp()->GetPreferences().GetPath(MAIN_KEY, _T("Tool"), prefPath);
  
  mpToolLabel = new nuiLabel(prefPath.GetPathName()); 
  mpToolLabel->SetDecoration(pDeco, eDecorationBorder);
  pBox->SetCell(0, mpToolLabel);
  pBox->SetCellExpand(0, nuiExpandShrinkAndGrow);
  nuiButton* pBtn = new nuiButton(_T("browse"));
  pBox->SetCell(1, pBtn);
  mEventSink.Connect(pBtn->Activated, &Generator::OnBrowseToolRequest);
  
  
  
  // separation line
  pMainBox->AddCell(NULL);
  pMainBox->SetCellPixels(pMainBox->GetNbCells()-1, 10);

  
  //*******************************************************************
  // bloc for source directory
  //
  pBloc = new nuiVBox(0);
  pBloc->SetExpand(nuiExpandShrinkAndGrow);
  pMainBox->AddCell(pBloc);
  
  pBloc->AddCell(new nuiLabel(_T("directory for graphic resources:")));
  pBloc->SetCellExpand(pBloc->GetNbCells()-1, nuiExpandShrinkAndGrow);  
  
  pBox = new nuiHBox(2);
  pBloc->AddCell(pBox);
  
  GetApp()->GetPreferences().GetPath(MAIN_KEY, _T("Source"), prefPath);
  
  mpSourceLabel = new nuiLabel(prefPath.GetPathName());  
  mpSourceLabel->SetDecoration(pDeco, eDecorationBorder);
  pBox->SetCell(0, mpSourceLabel);
  pBox->SetCellExpand(0, nuiExpandShrinkAndGrow);
  pBtn = new nuiButton(_T("browse"));
  pBox->SetCell(1, pBtn);
  mEventSink.Connect(pBtn->Activated, &Generator::OnBrowseSourceRequest);
  

  // separation line
  pMainBox->AddCell(NULL);
  pMainBox->SetCellPixels(pMainBox->GetNbCells()-1, 10);
  
  // start button
  pBtn = new nuiButton(_T("generate code"));
  mEventSink.Connect(pBtn->Activated, &Generator::OnStart);
  pMainBox->AddCell(pBtn, nuiCenter);
}



Generator::~Generator()
{

}


bool Generator::OnBrowseToolRequest(const nuiEvent& rEvent)
{
  int X,Y;
  GetApp()->GetMainWindow()->GetNGLWindow()->GetPosition(X, Y);
  GetApp()->GetMainWindow()->SetSize(APP_FULL_WIDTH,APP_FULL_HEIGHT);
  GetApp()->GetMainWindow()->GetNGLWindow()->SetPosition(X - (APP_FULL_WIDTH - APP_WIDTH)/2, Y - (APP_FULL_HEIGHT - APP_HEIGHT)/2);
  GetApp()->GetMainWindow()->UpdateLayout();
  
  mTimerSink.Connect(mTimer.Tick, &Generator::OnBrowseTool);
  mTimer.Start();
  
  return true;
}


bool Generator::OnBrowseTool(const nuiEvent& rEvent)
{
  mTimer.Stop();
  mTimerSink.Disconnect(mTimer.Tick, &Generator::OnBrowseTool);
  
  nuiDialogSelectFile* pDialog = new 
  nuiDialogSelectFile((nuiMainWindow*)GetMainWindow(), _T("SELECT THE GENERATOR TOOL COMMAND LINE"), 
                      nglPath(ePathUser), nglPath(_T("/")), nglString::Null, _T("*"), false);
  mEventSink.Connect(pDialog->FileSelected, &Generator::OnToolSelected, (void*)pDialog);
  pDialog->DoModal();
  return true;
}




bool Generator::OnBrowseSourceRequest(const nuiEvent& rEvent)
{
  int X,Y;
  GetApp()->GetMainWindow()->GetNGLWindow()->GetPosition(X, Y);
  GetApp()->GetMainWindow()->SetSize(APP_FULL_WIDTH,APP_FULL_HEIGHT);
  GetApp()->GetMainWindow()->GetNGLWindow()->SetPosition(X - (APP_FULL_WIDTH - APP_WIDTH)/2, Y - (APP_FULL_HEIGHT - APP_HEIGHT)/2);
  GetApp()->GetMainWindow()->UpdateLayout();
  
  mTimerSink.Connect(mTimer.Tick, &Generator::OnBrowseSource);
  mTimer.Start();

  return true;
}

bool Generator::OnBrowseSource(const nuiEvent& rEvent)
{
  mTimer.Stop();
  mTimerSink.Disconnect(mTimer.Tick, &Generator::OnBrowseSource);

  nuiDialogSelectDirectory* pDialog = new nuiDialogSelectDirectory((nuiMainWindow*)GetMainWindow(), _T("SELECT THE GRAPHIC RESOURCES DIRECTORY"), nglPath(ePathUser), nglPath(_T("/")));
  mEventSink.Connect(pDialog->DirectorySelected, &Generator::OnSourceSelected, (void*)pDialog);
  pDialog->DoModal();
  return true;
}



bool Generator::OnToolSelected(const nuiEvent& rEvent)
{
  nuiDialogSelectFile* pDialog = (nuiDialogSelectFile*)rEvent.mpUser;
  nglPath path = pDialog->GetSelectedFile();
  mpToolLabel->SetText(path.GetPathName());
  
  
  int X,Y;
  GetApp()->GetMainWindow()->GetNGLWindow()->GetPosition(X,Y);
  GetApp()->GetMainWindow()->SetSize(APP_WIDTH,APP_HEIGHT);
  GetApp()->GetMainWindow()->GetNGLWindow()->SetPosition(X + (APP_FULL_WIDTH - APP_WIDTH)/2, Y + (APP_FULL_HEIGHT - APP_HEIGHT)/2);    
  GetApp()->GetMainWindow()->UpdateLayout();  
  
  return false;
}

bool Generator::OnSourceSelected(const nuiEvent& rEvent)
{
  nuiDialogSelectDirectory* pDialog = (nuiDialogSelectDirectory*)rEvent.mpUser;
  nglPath path = pDialog->GetSelectedDirectory();
  mpSourceLabel->SetText(path.GetPathName());
  
  int X,Y;
  GetApp()->GetMainWindow()->GetNGLWindow()->GetPosition(X,Y);
  GetApp()->GetMainWindow()->SetSize(APP_WIDTH,APP_HEIGHT);
  GetApp()->GetMainWindow()->GetNGLWindow()->SetPosition(X + (APP_FULL_WIDTH - APP_WIDTH)/2, Y + (APP_FULL_HEIGHT - APP_HEIGHT)/2);    
  GetApp()->GetMainWindow()->UpdateLayout();  

  return false;
}



bool Generator::OnStart(const nuiEvent& rEvent)
{
  // check tool
  nglPath tool(mpToolLabel->GetText());
  if (!tool.Exists() || !tool.IsLeaf())
  {
    nuiMessageBox* pBox = new nuiMessageBox((nuiMainWindow*)GetMainWindow(), _T("oups"), _T("the generator tool command line path is not valid."), eMB_OK);
    pBox->QueryUser();
    return true;
  }
  
  
  // check source directory
  nglPath source(mpSourceLabel->GetText());
  nglPath pngSource = source + nglPath(_T("png"));
  
  if (!source.Exists() || source.IsLeaf() || !pngSource.Exists() || pngSource.IsLeaf())
  {
    nuiMessageBox* pBox = new nuiMessageBox((nuiMainWindow*)GetMainWindow(), _T("oups"), _T("the graphic resources path is not valid.\nIt must contain a 'png' folder,\nwith all the graphic resources inside."), eMB_OK);
    pBox->QueryUser();
    return true;
  }
  
  GetApp()->GetPreferences().SetPath(MAIN_KEY, _T("Tool"), tool);
  GetApp()->GetPreferences().SetPath(MAIN_KEY, _T("Source"), source);
  

  
  // empty code source folder
  nglPath codeSource = source + nglPath(_T("src"));
  if (codeSource.Exists())
  {
    nglString cmd;
    cmd.Format(_T("rm %ls/*.cpp"), codeSource.GetChars());
    system(cmd.GetStdString().c_str());
    cmd.Format(_T("rm %ls/*.h"), codeSource.GetChars());
    system(cmd.GetStdString().c_str());
  }
  // else create the folder
  else codeSource.Create();
  
  
  // delete "includer file"
  nglPath includeSource = nglPath(source.GetChars() + nglString(_T(".h")));
  if (includeSource.Exists())
  {
    nglString cmd;
    cmd.Format(_T("rm %ls"), includeSource.GetChars());
    system(cmd.GetStdString().c_str());
  }
  
  
  // parse png file list
  std::list<nglPath> children;
  pngSource.GetChildren(&children);
  
  std::list<nglPath>::iterator it;
  for (it = children.begin(); it != children.end(); ++it)
  {
    const nglPath& child = *it;
    if (child.GetExtension().Compare(_T("png"), false))
      continue;
    
    NGL_OUT(_T("path '%ls'\n"), child.GetChars());
    
    nglPath node = nglPath(child.GetNodeName());
    
    nglString nodeStr = node.GetPathName();
    nodeStr.DeleteRight(node.GetExtension().GetLength() +1);

    nglPath destPath = codeSource + nglPath(nodeStr);
    
    // and call the generator tool to create .cpp and .h files
    nglString cmd;
    cmd.Format(_T("%ls %ls %ls"), tool.GetChars(), child.GetChars(), destPath.GetChars());
    system(cmd.GetStdString().c_str());
  }
  
  
  // create the "includer file"
  nglPath includerPath = nglPath(source.GetPathName() + _T(".h"));
  char* includer_str = 
  "/*\n"
  "NUI3 - C++ cross-platform GUI framework for OpenGL based applications\n"
  "Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron\n"
  "\n"
  "licence: see nui3/LICENCE.TXT\n"
  "*/\n"
  "\n"
  "\n"
  "#pragma once\n"
  "\n";
  nglString includerStr(includer_str);
  
  for (it = children.begin(); it != children.end(); ++it)
  {
    nglPath child = *it;
    if (child.GetExtension().Compare(_T("png"), false))
      continue;
    
    nglPath node = nglPath(child.GetNodeName());
    
    nglString nodeStr = node.GetPathName();
    nodeStr.DeleteRight(node.GetExtension().GetLength() +1);
    
    nglPath destPath = codeSource + nglPath(nodeStr);
    destPath = nglPath(destPath.GetPathName() + _T(".h"));

    destPath.MakeRelativeTo(includerPath.GetParent());
    
    nglString tmp;
    tmp.Format(_T("#include \"%ls\"\n"), destPath.GetChars());
    includerStr.Append(tmp);
  }
  
  // write the "includer file" on disk
  nglIOStream* ostream = includerPath.OpenWrite();
  NGL_ASSERT(ostream);
  ostream->WriteText(includerStr);
  delete ostream;
  
  
  nuiMessageBox* pBox = new nuiMessageBox((nuiMainWindow*)GetMainWindow(), _T("embed_graphics"), _T("process successfull!"), eMB_OK);
  pBox->QueryUser();

  
  return true;
}

