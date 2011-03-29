/*==============================================================================

  Program: 3D Slicer

  Copyright (c) 2010 Kitware Inc.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// MRMLDisplayableManager includes
#include <vtkMRMLDisplayableManagerGroup.h>
#include <vtkMRMLViewDisplayableManager.h>
#include <vtkMRMLSliceModelDisplayableManager.h>
#include <vtkMRMLSliceViewDisplayableManagerFactory.h>

// MRMLLogic includes
#include <vtkMRMLApplicationLogic.h>
#include <vtkMRMLSliceLogic.h>

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLSliceNode.h>

// VTK includes
#include <vtkRegressionTestImage.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkErrorCode.h>
#include <vtkInteractorEventRecorder.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>

// STD includes
#include <cstdlib>
#include <iostream>
#include <iterator>

#include "TestingMacros.h"

// Convenient macro
#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

//----------------------------------------------------------------------------
int vtkMRMLSliceModelDisplayableManagerTest1(int argc, char* argv[])
{
  // Renderer, RenderWindow and Interactor
  vtkRenderer* rr = vtkRenderer::New();
  vtkRenderWindow* rw = vtkRenderWindow::New();
  vtkRenderWindowInteractor* ri = vtkRenderWindowInteractor::New();
  rw->SetSize(600, 600);

  rw->SetMultiSamples(0); // Ensure to have the same test image everywhere

  rw->AddRenderer(rr);
  rw->SetInteractor(ri);

  // MRML scene
  vtkMRMLScene* scene = vtkMRMLScene::New();

  // Application logic - Handle creation of vtkMRMLSelectionNode and vtkMRMLInteractionNode
  vtkMRMLApplicationLogic* applicationLogic = vtkMRMLApplicationLogic::New();
  applicationLogic->SetMRMLScene(scene);
  // Pass through event handling once without observing the scene
  // allows any dependent nodes to be created
  applicationLogic->ProcessMRMLEvents(scene, vtkCommand::ModifiedEvent, 0);
  applicationLogic->SetAndObserveMRMLScene(scene);

  // Add ViewNode
  vtkMRMLSliceNode * viewNode = vtkMRMLSliceNode::New();
  viewNode->SetLayoutName("Red");
  scene->AddNode(viewNode);
  viewNode->Delete();

  vtkMRMLSliceLogic * sliceLogic = vtkMRMLSliceLogic::New();
  sliceLogic->SetName("Red");
  sliceLogic->SetMRMLScene(scene);
  sliceLogic->SetSliceNode(viewNode);
  sliceLogic->UpdateSliceNode();
  sliceLogic->UpdateSliceNodeFromLayout();
  sliceLogic->UpdateSliceCompositeNode();

  // Factory
  vtkMRMLSliceViewDisplayableManagerFactory * factory = vtkMRMLSliceViewDisplayableManagerFactory::New();

  // Check if GetRegisteredDisplayableManagerCount returns 0
  if (factory->GetRegisteredDisplayableManagerCount() != 0)
    {
    std::cerr << "Expected RegisteredDisplayableManagerCount: 0" << std::endl;
    std::cerr << "Current RegisteredDisplayableManagerCount:"
        << factory->GetRegisteredDisplayableManagerCount() << std::endl;
    return EXIT_FAILURE;
    }

  factory->RegisterDisplayableManager("vtkMRMLSliceModelDisplayableManager");

  // Check if GetRegisteredDisplayableManagerCount returns 1
  if (factory->GetRegisteredDisplayableManagerCount() != 1)
    {
    std::cerr << "Expected RegisteredDisplayableManagerCount: 2" << std::endl;
    std::cerr << "Current RegisteredDisplayableManagerCount:"
        << factory->GetRegisteredDisplayableManagerCount() << std::endl;
    return EXIT_FAILURE;
    }

  vtkMRMLDisplayableManagerGroup * displayableManagerGroup =
    factory->InstantiateDisplayableManagers(rr);

  if (!displayableManagerGroup)
    {
    std::cerr << "Failed to instantiate Displayable Managers using "
        << "InstantiateDisplayableManagers" << std::endl;
    return EXIT_FAILURE;
    }

  // Check if GetDisplayableManagerCount returns 2
  if (displayableManagerGroup->GetDisplayableManagerCount() != 1)
    {
    std::cerr << "Check displayableManagerGroup->GetDisplayableManagerCount()" << std::endl;
    std::cerr << "Expected DisplayableManagerCount: 2" << std::endl;
    std::cerr << "Current DisplayableManagerCount:"
      << displayableManagerGroup->GetDisplayableManagerCount() << std::endl;
    return EXIT_FAILURE;
    }

  // Assign ViewNode
  displayableManagerGroup->SetMRMLDisplayableNode(viewNode);


  if (displayableManagerGroup)
    {
    displayableManagerGroup->Delete();
    }
  factory->Delete();
  applicationLogic->Delete();
  sliceLogic->Delete();
  scene->Delete();
  rr->Delete();
  rw->Delete();
  ri->Delete();

  return EXIT_SUCCESS;
}
