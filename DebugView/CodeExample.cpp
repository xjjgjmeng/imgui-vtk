﻿#include "CodeExample.h"

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
#include <vtkRendererCollection.h>
#include <vtkCoordinate.h>
#include <vtkCubeSource.h>
#include <vtkConeSource.h>
#include <vtkAbstractPicker.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkCameraOrientationWidget.h>

#include <imgui.h>

#include <VtkViewer.h>

#include "Common.h"

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void orientationMarker()
{
	static VtkViewer vtkViewer;
	static bool init = false;

	class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera {
	public:
		static MouseInteractorStyle* New()
		{
			return new MouseInteractorStyle;
		}

		void OnLeftButtonDown() override
		{
			vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
		}
	};

	static auto ca = vtkSmartPointer<vtkActor>::New();
	static double pos[3]{};
	if (ImGui::DragScalarN("pos", ImGuiDataType_Double, pos, 3))
	{
		ca->SetPosition(pos);
	}
	if (ImGui::Button("ActorRotateX"))
	{
		ca->RotateX(10);
	}
	ImGui::SameLine();
	if (ImGui::Button("ActorRotateY"))
	{
		ca->RotateX(10);
	}
	ImGui::SameLine();
	if (ImGui::Button("ActorRotateZ"))
	{
		ca->RotateZ(10);
	}
	if (ImGui::Button("CameraRoll"))
	{
		vtkViewer.getRenderer()->GetActiveCamera()->Roll(3);
	}

	if (!init)
	{
		init = true;

		vtkNew<vtkNamedColors> colors;

		// create a rendering window and renderer;
		auto ren = vtkViewer.getRenderer();;
		auto renWin = vtkViewer.getRenderWindow();
		renWin->AddRenderer(ren);
		renWin->SetWindowName("OrientationMarkerWidget");

		// create a renderwindowinteractor;
		auto iren = vtkViewer.getInteractor();
		iren->SetRenderWindow(renWin);

		static auto cube = vtkSmartPointer<vtkCubeSource>::New();
		cube->SetXLength(200);
		cube->SetYLength(200);
		cube->SetZLength(200);
		cube->Update();
		static auto cm = vtkSmartPointer<vtkPolyDataMapper>::New();
		cm->SetInputConnection(cube->GetOutputPort());
		ca->SetMapper(cm);
		ca->GetProperty()->SetColor(colors->GetColor3d("BurlyWood").GetData());
		ca->GetProperty()->EdgeVisibilityOn();
		ca->GetProperty()->SetEdgeColor(colors->GetColor3d("Red").GetData());

		auto callback = vtkSmartPointer<vtkCallbackCommand>::New();
		callback->SetCallback([](vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
			{
				auto ca = static_cast<vtkActor*>(caller);
				double mybounds[6];
				ca->GetBounds(mybounds);
				auto pcenter = ca->GetCenter();
				auto c0 = pcenter[0];
				auto c1 = pcenter[1];
				auto c2 = pcenter[2];

				double pos[3];
				ca->GetPosition(pos);

				double o[3];
				ca->GetOrigin(o);

				std::cout << "bounds:\t" << mybounds[0] << '\t' << mybounds[1] << '\t' << mybounds[2] << '\t' << mybounds[3] << '\t' << mybounds[4] << '\t' << mybounds[5] << std::endl;
				std::cout << "pos:\t" << pos[0] << '\t' << pos[1] << '\t' << pos[2] << std::endl;
				std::cout << "center:\t" << c0 << '\t' << c1 << '\t' << c2 << std::endl;
				std::cout << "origin:\t" << o[0] << '\t' << o[1] << '\t' << o[2] << '\n' << std::endl;
			});
		ca->AddObserver(vtkCommand::ModifiedEvent, callback);

		ren->AddActor(ca);
		ren->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

		vtkNew<vtkAnnotatedCubeActor> axesActor;
		axesActor->SetXPlusFaceText("L");
		axesActor->SetXMinusFaceText("R");
		axesActor->SetYMinusFaceText("I");
		axesActor->SetYPlusFaceText("S");
		axesActor->SetZMinusFaceText("P");
		axesActor->SetZPlusFaceText("A");
		axesActor->GetTextEdgesProperty()->SetColor(
			colors->GetColor3d("Yellow").GetData());
		//axesActor->GetTextEdgesProperty()->SetLineWidth(2);
		axesActor->GetCubeProperty()->SetColor(colors->GetColor3d("Blue").GetData());
		static vtkNew<vtkOrientationMarkerWidget> axes;
		axes->SetOrientationMarker(axesActor);
		axes->SetInteractor(iren);
		axes->EnabledOn();
		axes->SetViewport(0.9, 0.8, 1., 1.);
		//axes->InteractiveOn();
		vtkSmartPointer<MouseInteractorStyle> style = vtkSmartPointer<MouseInteractorStyle>::New();
		vtkViewer.getInteractor()->SetInteractorStyle(style);
		ren->ResetCamera();

		ren->GetActiveCamera()->Azimuth(45);
		ren->GetActiveCamera()->Elevation(30);

		vtkViewer.getRenderer()->SetBackground(0, 0, 0);
	}

	vtkViewer.render();
}

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

void coordinateTest()
{
	static VtkViewer myView;
	static bool init = false;

	static int eventPosition[2]{};
	static double worldPosition[3]{};
	static double worldPosition_[3]{};

	if (!init)
	{
		init = true;

		auto l = vtkSmartPointer<vtkCylinderSource>::New();
		auto lmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		lmapper->SetInputConnection(l->GetOutputPort());
		auto lactor = vtkSmartPointer<vtkActor>::New();
		lactor->SetMapper(lmapper);
		auto lren = myView.getRenderer();
		lren->AddActor(lactor);
		myView.getRenderer()->SetBackground(0.1, 0, 0);

		auto callback = vtkSmartPointer<vtkCallbackCommand>::New();
		callback->SetCallback([](vtkObject* caller, unsigned long eventId, void* clientData, void* callData)
			{
				vtkRenderWindowInteractor* interactor = static_cast<vtkRenderWindowInteractor*>(caller);
				interactor->GetEventPosition(eventPosition[0], eventPosition[1]);
				vtkRenderer* renderer = interactor->FindPokedRenderer(eventPosition[0], eventPosition[1]);
				if (renderer != nullptr)
				{
					vtkCoordinate* coordinate = vtkCoordinate::New();
					coordinate->SetCoordinateSystemToDisplay();
					coordinate->SetValue(eventPosition[0], eventPosition[1]);
					double* worldPoint = coordinate->GetComputedWorldValue(renderer);
					std::memcpy(worldPosition, worldPoint, sizeof(worldPosition));
					coordinate->Delete();
					// 另一种办法
					renderer->SetDisplayPoint(eventPosition[0], eventPosition[1], 0);
					renderer->DisplayToWorld();
					renderer->GetWorldPoint(worldPosition_);

					std::cout << "Picking pixel: " << interactor->GetEventPosition()[0]
						<< " " << interactor->GetEventPosition()[1] << std::endl;
					interactor->GetPicker()->Pick(interactor->GetEventPosition()[0],
						interactor->GetEventPosition()[1],
						0, // always zero.
						interactor->GetRenderWindow()
						->GetRenderers()
						->GetFirstRenderer());
					double picked[3];
					interactor->GetPicker()->GetPickPosition(picked);
					std::cout << "Picked value: " << picked[0] << " " << picked[1] << " "
						<< picked[2] << std::endl;
				}
			});
		myView.getInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent, callback);
	}

	ImGui::InputScalarN("EventPosition", ImGuiDataType_S32, eventPosition, static_cast<int>(std::size(eventPosition)), nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputScalarN("WorldPosition", ImGuiDataType_Double, worldPosition, static_cast<int>(std::size(worldPosition)), nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputScalarN("WorldPosition_", ImGuiDataType_Double, worldPosition_, static_cast<int>(std::size(worldPosition_)), nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);

	myView.render();
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

void cameratest()
{
	static VtkViewer vtkViewer;
	static bool init = false;

	static int myroll = 0;
	auto camera = vtkViewer.getRenderer()->GetActiveCamera();
	constexpr auto myOff = 1;
	myroll = vtkViewer.getRenderer()->GetActiveCamera()->GetRoll();
	static auto cube = vtkSmartPointer<vtkAnnotatedCubeActor>::New();

	if (!init)
	{
		init = true;

		vtkNew<vtkNamedColors> colors;
		auto renderer = vtkViewer.getRenderer();
#if 0
		auto cylinder = vtkSmartPointer<vtkConeSource>::New();
		cylinder->SetHeight(1.0);
		cylinder->SetRadius(1.0);
		cylinder->SetResolution(8);
		auto cylindermapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		cylindermapper->SetInputConnection(cylinder->GetOutputPort());
		auto cylinderactor = vtkSmartPointer<vtkActor>::New();
		cylinderactor->SetMapper(cylindermapper);
		renderer->AddActor(cylinderactor);
#endif
#if 0
		auto cube = vtkSmartPointer<vtkCubeSource>::New();
		cube->SetXLength(5);
		cube->SetYLength(5);
		cube->SetZLength(5);
		cube->Update();
		static auto cm = vtkSmartPointer<vtkPolyDataMapper>::New();
		cm->SetInputConnection(cube->GetOutputPort());
		auto ca = vtkSmartPointer<vtkActor>::New();
		ca->SetMapper(cm);
		ca->GetProperty()->SetColor(colors->GetColor3d("BurlyWood").GetData());
		ca->GetProperty()->EdgeVisibilityOn();
		ca->SetPosition(6, 0, 0);
		ca->GetProperty()->SetEdgeColor(colors->GetColor3d("Red").GetData());
		renderer->AddActor(ca);
#endif
		
#if 1
		cube->SetXPlusFaceText("L");
		cube->SetXMinusFaceText("R");
		cube->SetYMinusFaceText("I");
		cube->SetYPlusFaceText("S");
		cube->SetZMinusFaceText("P");
		cube->SetZPlusFaceText("A");
#endif
		cube->GetTextEdgesProperty()->SetColor(
			colors->GetColor3d("Red").GetData());
		//axesActor->GetTextEdgesProperty()->SetLineWidth(2);
		cube->GetCubeProperty()->SetColor(colors->GetColor3d("Blue").GetData());
		renderer->AddActor(cube);

		static vtkNew<vtkCameraOrientationWidget> camOrientManipulator;
		camOrientManipulator->SetParentRenderer(renderer);
		camOrientManipulator->On();

		vtkNew<vtkCallbackCommand> modifiedCallback;
		modifiedCallback->SetCallback([](vtkObject * caller,long unsigned int eventId,void* vtkNotUsed(clientData),void* vtkNotUsed(callData))
		{
			std::cout << caller->GetClassName() << " modified " << eventId << std::endl;
			vtkCamera* camera = static_cast<vtkCamera*>(caller);
		});
		//camera->AddObserver(vtkCommand::ModifiedEvent, modifiedCallback);

		renderer->SetBackground(0., 0., 0.);
		renderer->ResetCamera();
	}

	{
		ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.25f, ImGui::GetContentRegionAvail().y));
		{
			if (ImGui::DragInt("roll", &myroll, 1.f, -360, 360))
			{
				vtkViewer.getRenderer()->GetActiveCamera()->SetRoll(myroll);
			}
			if (ImGui::Button("Reset"))
			{
				vtkViewer.getRenderer()->ResetCamera();
			}
			ImGui::SameLine();
			if (ImGui::Button("OrthogonalizeViewUp"))
			{
				camera->OrthogonalizeViewUp();
			}
			ImGui::SameLine();
			if (ImGui::Button("ResetCameraClippingRange"))
			{
				vtkViewer.getRenderer()->ResetCameraClippingRange();
			}
			if (ImGui::Button("ComputeViewPlaneNormal"))
			{
				camera->ComputeViewPlaneNormal();
			}
			double viewup[3];
			camera->GetViewUp(viewup);
			if (ImGui::DragScalarN("ViewUp", ImGuiDataType_Double, viewup, 3, 0.01f))
			{
				camera->SetViewUp(viewup);
			}
			double pos[3];
			camera->GetPosition(pos);
			if (ImGui::DragScalarN("Position", ImGuiDataType_Double, pos, 3, 0.01f))
			{
				camera->SetPosition(pos);
			}
			double fp[3];
			camera->GetFocalPoint(fp);
			if (ImGui::DragScalarN("FocalPoint", ImGuiDataType_Double, fp, 3, 0.01f))
			{
				camera->SetFocalPoint(fp);
			}
			{
				static float near, far;
				double rangeVal[2];
				camera->GetClippingRange(rangeVal);
				near = rangeVal[0];
				far = rangeVal[1];
				if (ImGui::DragFloatRange2("ClippingRange", &near, &far, 0.1f, 0.0f, 100.0f, "Near: %lf", "Far: %lf"))
				{
					rangeVal[0] = near;
					rangeVal[1] = far;
					camera->SetClippingRange(rangeVal);
				}
			}
			auto orientation = camera->GetOrientation();
			ImGui::Text("Orientation:[%lf,%lf,%lf,%lf]", orientation[0], orientation[1], orientation[2], orientation[3]);
			auto dop = camera->GetDirectionOfProjection();
			ImGui::Text("DirectionOfProjection:[%lf,%lf,%lf]", dop[0], dop[1], dop[2]);
			{
				ImGui::Text("Distance:");
				ImGui::SameLine();
				float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
				ImGui::PushButtonRepeat(true);
				if (ImGui::ArrowButton("##SetDistance_left", ImGuiDir_Left)) { camera->SetDistance(camera->GetDistance() - myOff); }
				ImGui::SameLine(0.0f, spacing);
				if (ImGui::ArrowButton("##SetDistance_right", ImGuiDir_Right)) { camera->SetDistance(camera->GetDistance() + myOff); }
				ImGui::PopButtonRepeat();
				ImGui::SameLine();
				ImGui::Text("%lf", camera->GetDistance());
			}
			if (ImGui::TreeNode(u8"旋转"))
			{
				{
					{
						ImGui::Text("Roll:");
						ImGui::SameLine();
						float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
						ImGui::PushButtonRepeat(true);
						if (ImGui::ArrowButton("##Roll_left", ImGuiDir_Left)) { camera->Roll(-myOff); }
						ImGui::SameLine(0.0f, spacing);
						if (ImGui::ArrowButton("##Roll_right", ImGuiDir_Right)) { camera->Roll(myOff); }
						ImGui::PopButtonRepeat();
						ImGui::SameLine();
						ImGui::Text("%d", int(camera->GetRoll()));
						ImGui::SameLine();
						HelpMarker(u8R"(Rotate the camera about the direction of projection.
This will spin the camera about its axis.
围绕投影方向旋转相机，这将使相机绕其轴旋转)");
					}
					{
						ImGui::Text("Dolly:");
						ImGui::SameLine();
						float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
						ImGui::PushButtonRepeat(true);
						if (ImGui::ArrowButton("##Dolly_left", ImGuiDir_Left)) { camera->Dolly(-myOff); }
						ImGui::SameLine(0.0f, spacing);
						if (ImGui::ArrowButton("##Dolly_right", ImGuiDir_Right)) { camera->Dolly(myOff); }
						ImGui::PopButtonRepeat();
						ImGui::SameLine();
						HelpMarker(R"(Divide the camera's distance from the focal point by the given dolly value.
Use a value greater than one to dolly-in toward the focal point, and use a value less than one to dolly-out away from the focal point.)");
						//ImGui::SameLine();
						//ImGui::Text("%d", int(camera->GetDolly()));
					}
					{
						ImGui::Text("Elevation:");
						ImGui::SameLine();
						float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
						ImGui::PushButtonRepeat(true);
						if (ImGui::ArrowButton("##Elevation_left", ImGuiDir_Left)) { camera->Elevation(-myOff); }
						ImGui::SameLine(0.0f, spacing);
						if (ImGui::ArrowButton("##Elevation_right", ImGuiDir_Right)) { camera->Elevation(myOff); }
						ImGui::PopButtonRepeat();
						ImGui::SameLine();
						HelpMarker(R"(Rotate the camera about the cross product of the negative of the direction of projection and the view up vector, using the focal point as the center of rotation.
The result is a vertical rotation of the scene.)");
					}
					{
						ImGui::Text("Azimuth:");
						ImGui::SameLine();
						float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
						ImGui::PushButtonRepeat(true);
						if (ImGui::ArrowButton("##Azimuth_left", ImGuiDir_Left)) { camera->Azimuth(-myOff); }
						ImGui::SameLine(0.0f, spacing);
						if (ImGui::ArrowButton("##Azimuth_right", ImGuiDir_Right)) { camera->Azimuth(myOff); }
						ImGui::PopButtonRepeat();
						ImGui::SameLine();
						HelpMarker(u8R"(Rotate the camera about the view up vector centered at the focal point.
Note that the view up vector is whatever was set via SetViewUp, and is not necessarily perpendicular to the direction of projection.
The result is a horizontal rotation of the camera.
围绕以focalpoint为中心的viewup旋转相机（只有相机的position在变）
结果是相机的水平旋转)");
					}
					{
						ImGui::Text("Yaw:");
						ImGui::SameLine();
						float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
						ImGui::PushButtonRepeat(true);
						if (ImGui::ArrowButton("##Yaw_left", ImGuiDir_Left)) { camera->Yaw(-myOff); }
						ImGui::SameLine(0.0f, spacing);
						if (ImGui::ArrowButton("##Yaw_right", ImGuiDir_Right)) { camera->Yaw(myOff); }
						ImGui::PopButtonRepeat();
						ImGui::SameLine();
						HelpMarker(u8R"(Rotate the focal point about the view up vector, using the camera's position as the center of rotation.
Note that the view up vector is whatever was set via SetViewUp, and is not necessarily perpendicular to the direction of projection.
The result is a horizontal rotation of the scene.
使用相机position作为旋转中心，围绕viewup旋转focalpoint（只有focalpoint在变）
结果是场景的水平旋转)");
					}
					{
						ImGui::Text("Pitch:");
						ImGui::SameLine();
						float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
						ImGui::PushButtonRepeat(true);
						if (ImGui::ArrowButton("##Pitch_left", ImGuiDir_Left)) { camera->Pitch(-myOff); }
						ImGui::SameLine(0.0f, spacing);
						if (ImGui::ArrowButton("##Pitch_right", ImGuiDir_Right)) { camera->Pitch(myOff); }
						ImGui::PopButtonRepeat();
						ImGui::SameLine();
						HelpMarker(u8R"(Rotate the focal point about the cross product of the view up vector and the direction of projection, using the camera's position as the center of rotation.
The result is a vertical rotation of the camera.)");
					}
				}
				ImGui::TreePop();
			}

			{
				ImGui::Text("ViewAngle:");
				ImGui::SameLine();
				float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
				ImGui::PushButtonRepeat(true);
				if (ImGui::ArrowButton("##SetViewAngle_left", ImGuiDir_Left)) { camera->SetViewAngle(camera->GetViewAngle() - myOff); }
				ImGui::SameLine(0.0f, spacing);
				if (ImGui::ArrowButton("##SetViewAngle_right", ImGuiDir_Right)) { camera->SetViewAngle(camera->GetViewAngle() + myOff); }
				ImGui::PopButtonRepeat();
				ImGui::SameLine();
				ImGui::Text("%lf", camera->GetViewAngle());
			}
			{
				ImGui::Text("Zoom:");
				ImGui::SameLine();
				float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
				ImGui::PushButtonRepeat(true);
				if (ImGui::ArrowButton("##Zoom_left", ImGuiDir_Left)) { camera->Zoom(0.9); }
				ImGui::SameLine(0.0f, spacing);
				if (ImGui::ArrowButton("##Zoom_right", ImGuiDir_Right)) { camera->Zoom(1.1); }
				ImGui::PopButtonRepeat();
				ImGui::SameLine();
				HelpMarker(R"(In perspective mode, decrease the view angle by the specified factor.
In parallel mode, decrease the parallel scale by the specified factor.
A value greater than 1 is a zoom-in, a value less than 1 is a zoom-out.
@note This setting is ignored when UseExplicitProjectionTransformMatrix is true.)");
			}
			if (ImGui::Button(u8"水平翻转"))
			{
				camera->Azimuth(180.);
			}ImGui::SameLine();
			if (ImGui::Button(u8"垂直翻转"))
			{
				camera->Roll(180.);
				camera->Azimuth(180.);
			}ImGui::SameLine();
			if (ImGui::Button("Mirror"))
			{
				//double imageActorBound[6]{ 0 };
				//panData->getImageActor()->GetBounds(imageActorBound);
				//double* pCenter = panData->getImageActor()->GetCenter();
				//panData->getRenderer()->GetActiveCamera()->SetPosition(pCenter[0], pCenter[1], -1* panData->getRenderer()->GetActiveCamera()->GetDistance());
			}
			if (ImGui::Button(u8"旋转0"))
			{
				camera->SetViewUp(0.0, 1.0, 0.0);
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"旋转90"))
			{
				camera->SetViewUp(-1.0, 0.0, 0.0);
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"旋转180"))
			{
				camera->SetViewUp(0.0, -1.0, 0.0);
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"旋转270"))
			{
				camera->SetViewUp(1.0, 0.0, 0.0);
			}
			{
				const auto center = cube->GetCenter();
				const auto bounds = cube->GetBounds();
				constexpr auto ratio = 4;

				if (ImGui::Button("Superior"))
				{
					camera->SetViewUp(0, 0, -1);
					camera->SetPosition(center[0], center[1] + (bounds[3] - bounds[2])* ratio, center[2]);
					camera->SetFocalPoint(center);
					vtkViewer.getRenderer()->ResetCameraClippingRange();
					cube->SetOrigin(center);

				}
				ImGui::SameLine();
				if (ImGui::Button("Inferior"))
				{
					camera->SetViewUp(0, 0, 1);
					camera->SetPosition(center[0], center[1] - (bounds[3] - bounds[2]) * ratio, center[2]);
					camera->SetFocalPoint(center);
					vtkViewer.getRenderer()->ResetCameraClippingRange();
					cube->SetOrigin(center);
				}
				ImGui::SameLine();
				if (ImGui::Button("Left"))
				{
					camera->SetViewUp(0, 1, 0);
					camera->SetPosition(center[0] + (bounds[1] - bounds[0]) * ratio, center[1], center[2]);
					camera->SetFocalPoint(center);
					vtkViewer.getRenderer()->ResetCameraClippingRange();
					cube->SetOrigin(center);
				}
				ImGui::SameLine();
				if (ImGui::Button("Right"))
				{
					camera->SetViewUp(0, 1, 0);
					camera->SetPosition(center[0] - (bounds[1] - bounds[0]) * ratio, center[1], center[2]);
					camera->SetFocalPoint(center);
					vtkViewer.getRenderer()->ResetCameraClippingRange();
					cube->SetOrigin(center);
				}
				ImGui::SameLine();
				if (ImGui::Button("Anterior"))
				{
					camera->SetViewUp(0, 1, 0);
					camera->SetPosition(center[0], center[1], center[2] + (bounds[5] - bounds[4]) * ratio);
					camera->SetFocalPoint(center);
					vtkViewer.getRenderer()->ResetCameraClippingRange();
					cube->SetOrigin(center);
				}
				ImGui::SameLine();
				if (ImGui::Button("Posterior"))
				{
					camera->SetViewUp(0, 1, 0);
					camera->SetPosition(center[0], center[1], center[2] - (bounds[5] - bounds[4]) * ratio);
					camera->SetFocalPoint(center);
					vtkViewer.getRenderer()->ResetCameraClippingRange();
					cube->SetOrigin(center);
				}
			}
		}
		ImGui::EndChild();
	}
	ImGui::SameLine();
	{
		ImGui::BeginChild("ChildR");
		vtkViewer.render();
		ImGui::EndChild();
	}
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
			if (ImGui::BeginTabItem("Coordinate"))
			{
				coordinateTest();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Camera"))
			{
				cameratest();
				ImGui::EndTabItem();
			}
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
			if (ImGui::BeginTabItem(u8"方向指示"))
			{
				orientationMarker();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::Separator();
		ImGui::TreePop();
	}
}