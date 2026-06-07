#pragma once
#include <cstdint>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <windows.h>
#include <winhttp.h>
#include <nlohmann/json.hpp>
#pragma comment(lib, "winhttp.lib")
#include "oxorany/oxorany_include.h"

namespace offsets
{
    constexpr uintptr_t RenderToFakeDataModel = 0x38;
    constexpr std::uint32_t CFrame = 0x11c;

    inline uintptr_t Adornee = 0;
    inline uintptr_t Anchored = 0;
    inline uintptr_t AnimationId = 0;
    inline uintptr_t AttributeList = 0;
    inline uintptr_t AttributeToNext = 0;
    inline uintptr_t AttributeToValue = 0;
    inline uintptr_t AutoJumpEnabled = 0;
    inline uintptr_t BeamBrightness = 0;
    inline uintptr_t BeamColor = 0;
    inline uintptr_t BeamLightEmission = 0;
    inline uintptr_t BeamLightInfuence = 0;
    inline uintptr_t Camera = 0;
    inline uintptr_t CameraMaxZoomDistance = 0;
    inline uintptr_t CameraMinZoomDistance = 0;
    inline uintptr_t CameraMode = 0;
    inline uintptr_t CameraPos = 0;
    inline uintptr_t CameraRotation = 0;
    inline uintptr_t CameraSubject = 0;
    inline uintptr_t CameraType = 0;
    inline uintptr_t CanCollide = 0;
    inline uintptr_t CanTouch = 0;
    inline uintptr_t CharacterAppearanceId = 0;
    inline uintptr_t Children = 0;
    inline uintptr_t ChildrenEnd = 0;
    inline uintptr_t ClassDescriptor = 0;
    inline uintptr_t ClickDetectorMaxActivationDistance = 0;
    inline uintptr_t ClockTime = 0;
    inline uintptr_t CreatorId = 0;
    inline uintptr_t DataModelDeleterPointer = 0;
    inline uintptr_t DataModelPrimitiveCount = 0;
    inline uintptr_t DataModelToRenderView1 = 0;
    inline uintptr_t DataModelToRenderView2 = 0;
    inline uintptr_t DataModelToRenderView3 = 0;
    inline uintptr_t DecalTexture = 0;
    inline uintptr_t DecryptLuaState = 0;
    inline uintptr_t Deleter = 0;
    inline uintptr_t DeleterBack = 0;
    inline uintptr_t Dimensions = 0;
    inline uintptr_t DisplayName = 0;
    inline uintptr_t EvaluateStateMachine = 0;
    inline uintptr_t FOV = 0;
    inline uintptr_t FakeDataModelPointer = 0;
    inline uintptr_t FakeDataModelToDataModel = 0;
    inline uintptr_t FogColor = 0;
    inline uintptr_t FogEnd = 0;
    inline uintptr_t FogStart = 0;
    inline uintptr_t ForceNewAFKDuration = 0;
    inline uintptr_t FramePositionOffsetX = 0;
    inline uintptr_t FramePositionOffsetY = 0;
    inline uintptr_t FramePositionX = 0;
    inline uintptr_t FramePositionY = 0;
    inline uintptr_t FrameRotation = 0;
    inline uintptr_t FrameSizeX = 0;
    inline uintptr_t FrameSizeY = 0;
    inline uintptr_t GameId = 0;
    inline uintptr_t GameLoaded = 0;
    inline uintptr_t GetGlobalState = 0;
    inline uintptr_t Gravity = 0;
    inline uintptr_t Health = 0;
    inline uintptr_t HealthDisplayDistance = 0;
    inline uintptr_t HipHeight = 0;
    inline uintptr_t HumanoidDisplayName = 0;
    inline uintptr_t HumanoidState = 0;
    inline uintptr_t HumanoidStateId = 0;
    inline uintptr_t InputObject = 0;
    inline uintptr_t InsetMaxX = 0;
    inline uintptr_t InsetMaxY = 0;
    inline uintptr_t InsetMinX = 0;
    inline uintptr_t InsetMinY = 0;
    inline uintptr_t JobEnd = 0;
    inline uintptr_t JobId = 0;
    inline uintptr_t JobStart = 0;
    inline uintptr_t Job_Name = 0;
    inline uintptr_t JobsPointer = 0;
    inline uintptr_t JumpPower = 0;
    inline uintptr_t LocalPlayer = 0;
    inline uintptr_t LocalScriptByteCode = 0;
    inline uintptr_t LocalScriptBytecodePointer = 0;
    inline uintptr_t LocalScriptHash = 0;
    inline uintptr_t MaterialType = 0;
    inline uintptr_t MaxHealth = 0;
    inline uintptr_t MaxSlopeAngle = 0;
    inline uintptr_t MeshPartColor3 = 0;
    inline uintptr_t ModelInstance = 0;
    inline uintptr_t ModuleScriptByteCode = 0;
    inline uintptr_t ModuleScriptBytecodePointer = 0;
    inline uintptr_t ModuleScriptHash = 0;
    inline uintptr_t MoonTextureId = 0;
    inline uintptr_t MousePosition = 0;
    inline uintptr_t MouseSensitivity = 0;
    inline uintptr_t MoveDirection = 0;
    inline uintptr_t Name = 0;
    inline uintptr_t NameDisplayDistance = 0;
    inline uintptr_t NameSize = 0;
    inline uintptr_t OnDemandInstance = 0;
    inline uintptr_t OutdoorAmbient = 0;
    inline uintptr_t Parent = 0;
    inline uintptr_t PartSize = 0;
    inline uintptr_t Ping = 0;
    inline uintptr_t PlaceId = 0;
    inline uintptr_t PlayerConfigurerPointer = 0;
    inline uintptr_t Position = 0;
    inline uintptr_t Primitive = 0;
    inline uintptr_t PrimitiveGravity = 0;
    inline uintptr_t PrimitiveValidateValue = 0;
    inline uintptr_t PrimitivesPointer1 = 0;
    inline uintptr_t PrimitivesPointer2 = 0;
    inline uintptr_t ProximityPromptActionText = 0;
    inline uintptr_t ProximityPromptEnabled = 0;
    inline uintptr_t ProximityPromptGamepadKeyCode = 0;
    inline uintptr_t ProximityPromptHoldDuraction = 0;
    inline uintptr_t ProximityPromptMaxActivationDistance = 0;
    inline uintptr_t ProximityPromptMaxObjectText = 0;
    inline uintptr_t RenderJobToDataModel = 0;
    inline uintptr_t RenderJobToFakeDataModel = 0;
    inline uintptr_t RenderJobToRenderView = 0;
    inline uintptr_t RequireBypass = 0;
    inline uintptr_t RigType = 0;
    inline uintptr_t Rotation = 0;
    inline uintptr_t RunContext = 0;
    inline uintptr_t ScriptContext = 0;
    inline uintptr_t Sit = 0;
    inline uintptr_t SkyboxBk = 0;
    inline uintptr_t SkyboxDn = 0;
    inline uintptr_t SkyboxFt = 0;
    inline uintptr_t SkyboxLf = 0;
    inline uintptr_t SkyboxRt = 0;
    inline uintptr_t SkyboxUp = 0;
    inline uintptr_t SoundId = 0;
    inline uintptr_t StarCount = 0;
    inline uintptr_t StringLength = 0;
    inline uintptr_t SunTextureId = 0;
    inline uintptr_t TagList = 0;
    inline uintptr_t TaskSchedulerMaxFPS = 0;
    inline uintptr_t TaskSchedulerPointer = 0;
    inline uintptr_t Team = 0;
    inline uintptr_t TeamColor = 0;
    inline uintptr_t Tool_Grip_Position = 0;
    inline uintptr_t Transparency = 0;
    inline uintptr_t UserId = 0;
    inline uintptr_t Value = 0;
    inline uintptr_t Velocity = 0;
    inline uintptr_t ViewportSize = 0;
    inline uintptr_t VisualEngine = 0;
    inline uintptr_t VisualEnginePointer = 0;
    inline uintptr_t VisualEngineToDataModel1 = 0;
    inline uintptr_t VisualEngineToDataModel2 = 0;
    inline uintptr_t WalkSpeed = 0;
    inline uintptr_t WalkSpeedCheck = 0;
    inline uintptr_t WhitelistEncryption = 0;
    inline uintptr_t WhitelistEncryption2 = 0;
    inline uintptr_t WhitelistSetInsert = 0;
    inline uintptr_t WhitelistedPages = 0;
    inline uintptr_t WhitelistedThreads = 0;
    inline uintptr_t Workspace = 0;
    inline uintptr_t viewmatrix = 0;

    inline std::string RobloxVersionString;

    // =========================================================================
    // autoupdate()
    //
    // Strategy (fast after first run):
    //   1. GET offsets.imtheo.lol/roblox/version  (tiny plain-text, ~50 bytes)
    //   2. Read  %LOCALAPPDATA%\Layuh\offsets.version
    //   3. If they match AND offsets.json exists -> load from disk (no download)
    //   4. Otherwise  -> GET /offsets.json, save both files, apply
    //   5. If network fails -> fall back to stale cached JSON if present
    // =========================================================================
    inline void autoupdate()
    {
        // -- Resolve cache directory: %LOCALAPPDATA%\Layuh\ ---------------------
        char* pAppData = nullptr;
        size_t sz = 0;
        _dupenv_s(&pAppData, &sz, "LOCALAPPDATA");
        std::filesystem::path cacheDir =
            std::filesystem::path(pAppData ? pAppData : "") / "Layuh";
        if (pAppData) free(pAppData);
        std::error_code ec;
        std::filesystem::create_directories(cacheDir, ec);

        const std::string verPath  = (cacheDir / "offsets.version").string();
        const std::string jsonPath = (cacheDir / "offsets.json").string();

        // -- Helper: blocking WinHTTPS GET, returns body or "" ------------------
        auto winhttp_get = [](const wchar_t* host, const wchar_t* path) -> std::string
        {
            std::string body;
            HINTERNET hSess = WinHttpOpen(
                L"LayuhUpdater/1.0",
                WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
            if (!hSess) return body;

            HINTERNET hConn = WinHttpConnect(hSess, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
            if (!hConn) { WinHttpCloseHandle(hSess); return body; }

            HINTERNET hReq = WinHttpOpenRequest(
                hConn, L"GET", path,
                nullptr, WINHTTP_NO_REFERER,
                WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
            if (!hReq) {
                WinHttpCloseHandle(hConn);
                WinHttpCloseHandle(hSess);
                return body;
            }

            if (WinHttpSendRequest(hReq, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                   WINHTTP_NO_REQUEST_DATA, 0, 0, 0)
                && WinHttpReceiveResponse(hReq, nullptr))
            {
                DWORD avail = 0;
                while (WinHttpQueryDataAvailable(hReq, &avail) && avail > 0) {
                    std::string chunk(avail, '\0');
                    DWORD rd = 0;
                    if (!WinHttpReadData(hReq, &chunk[0], avail, &rd)) break;
                    body.append(chunk, 0, rd);
                }
            }

            WinHttpCloseHandle(hReq);
            WinHttpCloseHandle(hConn);
            WinHttpCloseHandle(hSess);
            return body;
        };

        // Helper: trim trailing CR/LF/space
        auto rtrim = [](std::string s) -> std::string {
            while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' '))
                s.pop_back();
            return s;
        };

        // -- 1. Fetch live Roblox version (fast, single small request) ----------
        printf("[offsets] Checking Roblox version...\n");
        std::string liveVer = rtrim(
            winhttp_get(L"offsets.imtheo.lol", L"/roblox/version"));

        // -- 2. Read cached version from disk -----------------------------------
        std::string cachedVer;
        {
            std::ifstream vf(verPath);
            if (vf) { std::getline(vf, cachedVer); }
            cachedVer = rtrim(cachedVer);
        }

        bool jsonExists = std::filesystem::exists(jsonPath, ec);

        std::string responseStr;

        if (!liveVer.empty() && liveVer == cachedVer && jsonExists)
        {
            // -- 3. Cache is fresh: load from disk (instant, no download) -------
            printf("[offsets] Cache up-to-date (%s). Loading from disk.\n",
                   liveVer.c_str());
            std::ifstream jf(jsonPath, std::ios::binary);
            responseStr.assign(std::istreambuf_iterator<char>(jf), {});
        }
        else
        {
            // -- 4. Download fresh offsets.json ---------------------------------
            printf("[offsets] Updating offsets for %s...\n",
                   liveVer.empty() ? "(unknown)" : liveVer.c_str());

            responseStr = winhttp_get(L"offsets.imtheo.lol", L"/offsets.json");

            if (!responseStr.empty()) {
                // Write JSON cache
                { std::ofstream jf(jsonPath, std::ios::binary | std::ios::trunc);
                  jf << responseStr; }
                // Write version cache
                if (!liveVer.empty()) {
                    std::ofstream vf(verPath, std::ios::trunc);
                    vf << liveVer;
                }
                printf("[offsets] Saved to %%LOCALAPPDATA%%\\Layuh\\\n");
            } else {
                // -- 5. Network failed: fall back to stale cache ----------------
                printf("[offsets] Download failed. Trying stale cache...\n");
                if (jsonExists) {
                    std::ifstream jf(jsonPath, std::ios::binary);
                    responseStr.assign(std::istreambuf_iterator<char>(jf), {});
                    if (!responseStr.empty())
                        printf("[offsets] Using stale cache.\n");
                }
            }
        }

        if (responseStr.empty()) {
            printf("[offsets] No offsets available. All offsets will be 0.\n");
            return;
        }

        // -- Parse JSON ---------------------------------------------------------
        auto j = nlohmann::json::parse(responseStr, nullptr, false);
        if (j.is_discarded()) {
            printf("[offsets] JSON parse failed.\n");
            return;
        }

        // Extract version string for display
        if (j.contains("Roblox Version"))
            RobloxVersionString = j["Roblox Version"].get<std::string>();
        else if (j.contains("RobloxVersion"))
            RobloxVersionString = j["RobloxVersion"].get<std::string>();

        // -- Apply: support BOTH new nested format AND old flat hex-string format
        if (j.contains("Offsets") && j["Offsets"].is_object())
        {
            // New format: {"Offsets":{"ClassName":{"FieldName":404,...},...}}
            const auto& om = j["Offsets"];
            auto geto = [&](const char* field) -> uintptr_t {
                for (auto& [cls, fields] : om.items())
                    if (fields.is_object() && fields.contains(field))
                        return fields[field].get<uintptr_t>();
                return 0;
            };
#define APPLY_N(n) n = geto(#n)
            APPLY_N(Adornee); APPLY_N(Anchored); APPLY_N(AnimationId);
            APPLY_N(AttributeList); APPLY_N(AttributeToNext); APPLY_N(AttributeToValue);
            APPLY_N(AutoJumpEnabled); APPLY_N(BeamBrightness); APPLY_N(BeamColor);
            APPLY_N(BeamLightEmission); APPLY_N(BeamLightInfuence); APPLY_N(Camera);
            APPLY_N(CameraMaxZoomDistance); APPLY_N(CameraMinZoomDistance); APPLY_N(CameraMode);
            APPLY_N(CameraPos); APPLY_N(CameraRotation); APPLY_N(CameraSubject);
            APPLY_N(CameraType); APPLY_N(CanCollide); APPLY_N(CanTouch);
            APPLY_N(CharacterAppearanceId); APPLY_N(Children); APPLY_N(ChildrenEnd);
            APPLY_N(ClassDescriptor); APPLY_N(ClickDetectorMaxActivationDistance);
            APPLY_N(ClockTime); APPLY_N(CreatorId); APPLY_N(DataModelDeleterPointer);
            APPLY_N(DataModelPrimitiveCount); APPLY_N(DataModelToRenderView1);
            APPLY_N(DataModelToRenderView2); APPLY_N(DataModelToRenderView3);
            APPLY_N(DecalTexture); APPLY_N(DecryptLuaState); APPLY_N(Deleter);
            APPLY_N(DeleterBack); APPLY_N(Dimensions); APPLY_N(DisplayName);
            APPLY_N(EvaluateStateMachine); APPLY_N(FOV); APPLY_N(FakeDataModelPointer);
            APPLY_N(FakeDataModelToDataModel); APPLY_N(FogColor); APPLY_N(FogEnd);
            APPLY_N(FogStart); APPLY_N(ForceNewAFKDuration); APPLY_N(FramePositionOffsetX);
            APPLY_N(FramePositionOffsetY); APPLY_N(FramePositionX); APPLY_N(FramePositionY);
            APPLY_N(FrameRotation); APPLY_N(FrameSizeX); APPLY_N(FrameSizeY);
            APPLY_N(GameId); APPLY_N(GameLoaded); APPLY_N(GetGlobalState);
            APPLY_N(Gravity); APPLY_N(Health); APPLY_N(HealthDisplayDistance);
            APPLY_N(HipHeight); APPLY_N(HumanoidDisplayName); APPLY_N(HumanoidState);
            APPLY_N(HumanoidStateId); APPLY_N(InputObject); APPLY_N(InsetMaxX);
            APPLY_N(InsetMaxY); APPLY_N(InsetMinX); APPLY_N(InsetMinY);
            APPLY_N(JobEnd); APPLY_N(JobId); APPLY_N(JobStart); APPLY_N(Job_Name);
            APPLY_N(JobsPointer); APPLY_N(JumpPower); APPLY_N(LocalPlayer);
            APPLY_N(LocalScriptByteCode); APPLY_N(LocalScriptBytecodePointer);
            APPLY_N(LocalScriptHash); APPLY_N(MaterialType); APPLY_N(MaxHealth);
            APPLY_N(MaxSlopeAngle); APPLY_N(MeshPartColor3); APPLY_N(ModelInstance);
            APPLY_N(ModuleScriptByteCode); APPLY_N(ModuleScriptBytecodePointer);
            APPLY_N(ModuleScriptHash); APPLY_N(MoonTextureId); APPLY_N(MousePosition);
            APPLY_N(MouseSensitivity); APPLY_N(MoveDirection); APPLY_N(Name);
            APPLY_N(NameDisplayDistance); APPLY_N(NameSize); APPLY_N(OnDemandInstance);
            APPLY_N(OutdoorAmbient); APPLY_N(Parent); APPLY_N(PartSize);
            APPLY_N(Ping); APPLY_N(PlaceId); APPLY_N(PlayerConfigurerPointer);
            APPLY_N(Position); APPLY_N(Primitive); APPLY_N(PrimitiveGravity);
            APPLY_N(PrimitiveValidateValue); APPLY_N(PrimitivesPointer1);
            APPLY_N(PrimitivesPointer2); APPLY_N(ProximityPromptActionText);
            APPLY_N(ProximityPromptEnabled); APPLY_N(ProximityPromptGamepadKeyCode);
            APPLY_N(ProximityPromptHoldDuraction); APPLY_N(ProximityPromptMaxActivationDistance);
            APPLY_N(ProximityPromptMaxObjectText); APPLY_N(RenderJobToDataModel);
            APPLY_N(RenderJobToFakeDataModel); APPLY_N(RenderJobToRenderView);
            APPLY_N(RequireBypass); APPLY_N(RigType); APPLY_N(Rotation);
            APPLY_N(RunContext); APPLY_N(ScriptContext); APPLY_N(Sit);
            APPLY_N(SkyboxBk); APPLY_N(SkyboxDn); APPLY_N(SkyboxFt);
            APPLY_N(SkyboxLf); APPLY_N(SkyboxRt); APPLY_N(SkyboxUp);
            APPLY_N(SoundId); APPLY_N(StarCount); APPLY_N(StringLength);
            APPLY_N(SunTextureId); APPLY_N(TagList); APPLY_N(TaskSchedulerMaxFPS);
            APPLY_N(TaskSchedulerPointer); APPLY_N(Team); APPLY_N(TeamColor);
            APPLY_N(Tool_Grip_Position); APPLY_N(Transparency); APPLY_N(UserId);
            APPLY_N(Value); APPLY_N(Velocity); APPLY_N(ViewportSize);
            APPLY_N(VisualEngine); APPLY_N(VisualEnginePointer);
            APPLY_N(VisualEngineToDataModel1); APPLY_N(VisualEngineToDataModel2);
            APPLY_N(WalkSpeed); APPLY_N(WalkSpeedCheck); APPLY_N(WhitelistEncryption);
            APPLY_N(WhitelistEncryption2); APPLY_N(WhitelistSetInsert);
            APPLY_N(WhitelistedPages); APPLY_N(WhitelistedThreads);
            APPLY_N(Workspace); APPLY_N(viewmatrix);
#undef APPLY_N
        }
        else
        {
            // Legacy flat format: values are hex strings
#define UPDATE_OFFSET(name) \
            if (j.contains(#name)) { \
                std::stringstream _ss; \
                _ss << std::hex << j[#name].get<std::string>(); \
                _ss >> offsets::name; \
            }
            UPDATE_OFFSET(Adornee) UPDATE_OFFSET(Anchored) UPDATE_OFFSET(AnimationId)
            UPDATE_OFFSET(AttributeList) UPDATE_OFFSET(AttributeToNext) UPDATE_OFFSET(AttributeToValue)
            UPDATE_OFFSET(AutoJumpEnabled) UPDATE_OFFSET(BeamBrightness) UPDATE_OFFSET(BeamColor)
            UPDATE_OFFSET(BeamLightEmission) UPDATE_OFFSET(BeamLightInfuence) UPDATE_OFFSET(Camera)
            UPDATE_OFFSET(CameraMaxZoomDistance) UPDATE_OFFSET(CameraMinZoomDistance) UPDATE_OFFSET(CameraMode)
            UPDATE_OFFSET(CameraPos) UPDATE_OFFSET(CameraRotation) UPDATE_OFFSET(CameraSubject)
            UPDATE_OFFSET(CameraType) UPDATE_OFFSET(CanCollide) UPDATE_OFFSET(CanTouch)
            UPDATE_OFFSET(CharacterAppearanceId) UPDATE_OFFSET(Children) UPDATE_OFFSET(ChildrenEnd)
            UPDATE_OFFSET(ClassDescriptor) UPDATE_OFFSET(ClickDetectorMaxActivationDistance)
            UPDATE_OFFSET(ClockTime) UPDATE_OFFSET(CreatorId) UPDATE_OFFSET(DataModelDeleterPointer)
            UPDATE_OFFSET(DataModelPrimitiveCount) UPDATE_OFFSET(DataModelToRenderView1)
            UPDATE_OFFSET(DataModelToRenderView2) UPDATE_OFFSET(DataModelToRenderView3)
            UPDATE_OFFSET(DecalTexture) UPDATE_OFFSET(DecryptLuaState) UPDATE_OFFSET(Deleter)
            UPDATE_OFFSET(DeleterBack) UPDATE_OFFSET(Dimensions) UPDATE_OFFSET(DisplayName)
            UPDATE_OFFSET(EvaluateStateMachine) UPDATE_OFFSET(FOV) UPDATE_OFFSET(FakeDataModelPointer)
            UPDATE_OFFSET(FakeDataModelToDataModel) UPDATE_OFFSET(FogColor) UPDATE_OFFSET(FogEnd)
            UPDATE_OFFSET(FogStart) UPDATE_OFFSET(ForceNewAFKDuration) UPDATE_OFFSET(FramePositionOffsetX)
            UPDATE_OFFSET(FramePositionOffsetY) UPDATE_OFFSET(FramePositionX) UPDATE_OFFSET(FramePositionY)
            UPDATE_OFFSET(FrameRotation) UPDATE_OFFSET(FrameSizeX) UPDATE_OFFSET(FrameSizeY)
            UPDATE_OFFSET(GameId) UPDATE_OFFSET(GameLoaded) UPDATE_OFFSET(GetGlobalState)
            UPDATE_OFFSET(Gravity) UPDATE_OFFSET(Health) UPDATE_OFFSET(HealthDisplayDistance)
            UPDATE_OFFSET(HipHeight) UPDATE_OFFSET(HumanoidDisplayName) UPDATE_OFFSET(HumanoidState)
            UPDATE_OFFSET(HumanoidStateId) UPDATE_OFFSET(InputObject) UPDATE_OFFSET(InsetMaxX)
            UPDATE_OFFSET(InsetMaxY) UPDATE_OFFSET(InsetMinX) UPDATE_OFFSET(InsetMinY)
            UPDATE_OFFSET(JobEnd) UPDATE_OFFSET(JobId) UPDATE_OFFSET(JobStart) UPDATE_OFFSET(Job_Name)
            UPDATE_OFFSET(JobsPointer) UPDATE_OFFSET(JumpPower) UPDATE_OFFSET(LocalPlayer)
            UPDATE_OFFSET(LocalScriptByteCode) UPDATE_OFFSET(LocalScriptBytecodePointer)
            UPDATE_OFFSET(LocalScriptHash) UPDATE_OFFSET(MaterialType) UPDATE_OFFSET(MaxHealth)
            UPDATE_OFFSET(MaxSlopeAngle) UPDATE_OFFSET(MeshPartColor3) UPDATE_OFFSET(ModelInstance)
            UPDATE_OFFSET(ModuleScriptByteCode) UPDATE_OFFSET(ModuleScriptBytecodePointer)
            UPDATE_OFFSET(ModuleScriptHash) UPDATE_OFFSET(MoonTextureId) UPDATE_OFFSET(MousePosition)
            UPDATE_OFFSET(MouseSensitivity) UPDATE_OFFSET(MoveDirection) UPDATE_OFFSET(Name)
            UPDATE_OFFSET(NameDisplayDistance) UPDATE_OFFSET(NameSize) UPDATE_OFFSET(OnDemandInstance)
            UPDATE_OFFSET(OutdoorAmbient) UPDATE_OFFSET(Parent) UPDATE_OFFSET(PartSize)
            UPDATE_OFFSET(Ping) UPDATE_OFFSET(PlaceId) UPDATE_OFFSET(PlayerConfigurerPointer)
            UPDATE_OFFSET(Position) UPDATE_OFFSET(Primitive) UPDATE_OFFSET(PrimitiveGravity)
            UPDATE_OFFSET(PrimitiveValidateValue) UPDATE_OFFSET(PrimitivesPointer1)
            UPDATE_OFFSET(PrimitivesPointer2) UPDATE_OFFSET(ProximityPromptActionText)
            UPDATE_OFFSET(ProximityPromptEnabled) UPDATE_OFFSET(ProximityPromptGamepadKeyCode)
            UPDATE_OFFSET(ProximityPromptHoldDuraction) UPDATE_OFFSET(ProximityPromptMaxActivationDistance)
            UPDATE_OFFSET(ProximityPromptMaxObjectText) UPDATE_OFFSET(RenderJobToDataModel)
            UPDATE_OFFSET(RenderJobToFakeDataModel) UPDATE_OFFSET(RenderJobToRenderView)
            UPDATE_OFFSET(RequireBypass) UPDATE_OFFSET(RigType) UPDATE_OFFSET(Rotation)
            UPDATE_OFFSET(RunContext) UPDATE_OFFSET(ScriptContext) UPDATE_OFFSET(Sit)
            UPDATE_OFFSET(SkyboxBk) UPDATE_OFFSET(SkyboxDn) UPDATE_OFFSET(SkyboxFt)
            UPDATE_OFFSET(SkyboxLf) UPDATE_OFFSET(SkyboxRt) UPDATE_OFFSET(SkyboxUp)
            UPDATE_OFFSET(SoundId) UPDATE_OFFSET(StarCount) UPDATE_OFFSET(StringLength)
            UPDATE_OFFSET(SunTextureId) UPDATE_OFFSET(TagList) UPDATE_OFFSET(TaskSchedulerMaxFPS)
            UPDATE_OFFSET(TaskSchedulerPointer) UPDATE_OFFSET(Team) UPDATE_OFFSET(TeamColor)
            UPDATE_OFFSET(Tool_Grip_Position) UPDATE_OFFSET(Transparency) UPDATE_OFFSET(UserId)
            UPDATE_OFFSET(Value) UPDATE_OFFSET(Velocity) UPDATE_OFFSET(ViewportSize)
            UPDATE_OFFSET(VisualEngine) UPDATE_OFFSET(VisualEnginePointer)
            UPDATE_OFFSET(VisualEngineToDataModel1) UPDATE_OFFSET(VisualEngineToDataModel2)
            UPDATE_OFFSET(WalkSpeed) UPDATE_OFFSET(WalkSpeedCheck) UPDATE_OFFSET(WhitelistEncryption)
            UPDATE_OFFSET(WhitelistEncryption2) UPDATE_OFFSET(WhitelistSetInsert)
            UPDATE_OFFSET(WhitelistedPages) UPDATE_OFFSET(WhitelistedThreads)
            UPDATE_OFFSET(Workspace) UPDATE_OFFSET(viewmatrix)
#undef UPDATE_OFFSET
        }

        printf("[offsets] Applied successfully. Version: %s\n",
               RobloxVersionString.empty() ? "(unknown)" : RobloxVersionString.c_str());
    }
}
