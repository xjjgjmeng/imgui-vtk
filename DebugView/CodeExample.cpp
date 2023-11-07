#include "CodeExample.h"

#include <vtkImageData.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMapper3D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkNamedColors.h>
#include <vtkShortArray.h>
#include <vtkStructuredPoints.h>
#include <vtkContourFilter.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkCylinderSource.h>
#include <vtkOutlineFilter.h>
#include <vtkJPEGReader.h>
#include <vtkPlaneSource.h>
#include <vtkTexture.h>
#include <vtkLight.h>
#include <vtkCamera.h>

#include <imgui.h>

#include <VtkViewer.h>

#include "Common.h"

void leftRightScreen()
{
	static VtkViewer vtkViewer;
	static bool init = false;

	if (!init)
	{
		init = true;

		auto l = vtkSmartPointer<vtkCylinderSource>::New();
		auto lmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		lmapper->SetInputConnection(l->GetOutputPort());
		auto lactor = vtkSmartPointer<vtkActor>::New();
		lactor->SetMapper(lmapper);
		auto lren = vtkViewer.getRenderer();//vtkSmartPointer<vtkRenderer>::New();
		lren->AddActor(lactor);
		lren->SetViewport(0, 0, 0.5, 1);

		auto r = vtkSmartPointer<vtkCylinderSource>::New();
		auto rmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		rmapper->SetInputConnection(r->GetOutputPort());
		auto ractor = vtkSmartPointer<vtkActor>::New();
		ractor->SetMapper(rmapper);
		auto rren = vtkSmartPointer<vtkRenderer>::New();
		rren->AddActor(ractor);
		rren->SetViewport(0.5, 0, 1, 1);

		auto renw = vtkViewer.getRenderWindow();//vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
		//renw->AddRenderer(lren);
		renw->AddRenderer(rren);

		//vtkViewer.setRenderWindow(renw);
	}

	vtkViewer.render();
}

void planeAndTexture()
{
	static VtkViewer vtkViewer;
	static bool init = false;

	if (!init)
	{
		init = true;

		auto reader = vtkSmartPointer<vtkJPEGReader>::New();
		reader->SetFileName("./pic/Lenna.jpg");
		auto texture = vtkSmartPointer<vtkTexture>::New();
		texture->SetInputConnection(reader->GetOutputPort());
		texture->InterpolateOn();
		auto plane = vtkSmartPointer<vtkPlaneSource>::New();
		auto mapperr = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapperr->SetInputConnection(plane->GetOutputPort());
		auto actor = vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapperr);
		actor->SetTexture(texture);
		vtkViewer.addActor(actor);

		vtkViewer.getRenderer()->SetBackground(0, 0, 0);
	}

	vtkViewer.render();
}

void lighttest()
{
	static VtkViewer vtkViewer;
	static bool init = false;

	if (!init)
	{
		init = true;

		auto cylinder = vtkSmartPointer<vtkCylinderSource>::New();
		cylinder->SetHeight(3.0);
		cylinder->SetRadius(1.0);
		cylinder->SetResolution(3);
		auto cylindermapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		cylindermapper->SetInputConnection(cylinder->GetOutputPort());
		auto cylinderactor = vtkSmartPointer<vtkActor>::New();
		cylinderactor->SetMapper(cylindermapper);
		auto renderer = vtkViewer.getRenderer();
		renderer->AddActor(cylinderactor);
		renderer->SetBackground(0.1, 0.1, 0.1);

		auto mylight = vtkSmartPointer<vtkLight>::New();
		mylight->SetColor(0, 1, 0);
		mylight->SetPosition(0, 0, 1);
		mylight->SetFocalPoint(renderer->GetActiveCamera()->GetFocalPoint());
		renderer->AddLight(mylight);

		auto mylight1 = vtkSmartPointer<vtkLight>::New();
		mylight1->SetColor(1, 0, 0);
		mylight1->SetPosition(0, 0, -1);
		mylight1->SetFocalPoint(renderer->GetActiveCamera()->GetFocalPoint());
		renderer->AddLight(mylight1);
		renderer->ResetCamera();

		vtkViewer.getRenderer()->SetBackground(1, 1, 1);
	}

	vtkViewer.render();
}

void createImageData()
{
	static VtkViewer vtkViewer12;
	static bool init = false;

	if (!init)
	{
		init = true;
		auto img = vtkSmartPointer<vtkImageData>::New();
		// 设置图像的维度、原点、间隔等信息
		img->SetDimensions(256, 256, 1);
		img->SetOrigin(0.0, 0.0, 0.0);
		img->SetSpacing(1.0, 1.0, 1.0);

		// 分配内存并写入数据
		img->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
		unsigned char* data = static_cast<unsigned char*>(img->GetScalarPointer(0, 0, 0));

		for (int i = 0; i < 256; i++)
		{
			for (int j = 0; j < 256; j++)
			{
				data[i * 256 + j] = static_cast<unsigned char>(i + j);
			}
		}
		auto actor = vtkSmartPointer<vtkImageActor>::New();
		actor->SetInputData(img);
		vtkViewer12.addActor(actor);

		vtkViewer12.getRenderer()->SetBackground(0, 0, 0);
	}

	vtkViewer12.render();
}

void showOutline()
{
	static bool init = false;
	static VtkViewer vtkViewer12;
	static auto cylinder = vtkSmartPointer<vtkCylinderSource>::New();
	static auto outlineactor = vtkSmartPointer<vtkActor>::New();
	if (!init)
	{
		init = true;
		cylinder->SetResolution(18);
		auto cylindermapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		cylindermapper->SetInputConnection(cylinder->GetOutputPort());
		auto cylinderactor = vtkSmartPointer<vtkActor>::New();
		cylinderactor->SetMapper(cylindermapper);

		auto callback = vtkSmartPointer<vtkCallbackCommand>::New();
		callback->SetCallback([](vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
			{
				vtkCylinderSource* cylinderSource = static_cast<vtkCylinderSource*>(caller);
				int resolution = cylinderSource->GetResolution();
				std::cout << __func__ << '\t' << resolution << std::endl;
			});
		cylinder->AddObserver(vtkCommand::ModifiedEvent, callback);

		auto filter = vtkSmartPointer<vtkOutlineFilter>::New();
		filter->SetInputConnection(cylinder->GetOutputPort());
		auto outlinemapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		outlinemapper->SetInputConnection(filter->GetOutputPort());
		outlineactor->SetMapper(outlinemapper);

		vtkViewer12.addActor(cylinderactor);
		vtkViewer12.addActor(outlineactor);

		vtkViewer12.getRenderer()->SetBackground(0, 0, 0);
	}

	static int resolution = cylinder->GetResolution();
	if (ImGui::SliderInt("Resolution", &resolution, 3, 100))
	{
		cylinder->SetResolution(resolution);
	}
	static bool showOutline{ true };
	if (ImGui::Checkbox("ShwOutline", &showOutline))
	{
		if (showOutline)
		{
			vtkViewer12.addActor(outlineactor);
		}
		else
		{
			vtkViewer12.removeActor(outlineactor);
		}
	}
	vtkViewer12.render();
}



void renderExample()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
	if (ImGui::TreeNode("Basic"))
	{
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Cylinder"))
			{
				showOutline();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(u8"创建vtkImageData"))
			{
				createImageData();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(u8"左右屏"))
			{
				leftRightScreen();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(u8"平面和2D纹理"))
			{
				planeAndTexture();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(u8"灯光"))
			{
				lighttest();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::Separator();
		ImGui::TreePop();
	}
}