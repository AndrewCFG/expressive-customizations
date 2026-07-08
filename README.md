# Expressive Customizations
Fixes and customizations for ExpressiveText.

## Expressive Text Actor Shadows
In newer versions of Unreal Engine, Widget Components no longer cast shadows when using Virtual Shadow Maps. This means that Expressive Text Actors no longer cast shadows either. There is a [backlogged bug ticket](https://issues.unrealengine.com/issue/UE-351232) for the engine, but in the meantime, there are some updates that can be applied to re-create the shadows.

<img width="446" height="316" alt="picture of text casting shadows" src="https://github.com/user-attachments/assets/debdfb85-5016-4ba3-92ca-4bd37432b2cd" />

### Details
The included class extends AExpressiveTextActor to include a 2D plane with a texture applied from the Expressive Text WidgetComponent's Render Target. The plane is invisible in-game, but enables Hidden Shadows to cast the expected shadows. This works with text movement and animation by defaulting Shadow Cache Invalidation to Always. The invalidation setting is exposed within an "ExpressiveText" category and can be changed to something more performant if your text is static.

This was meant to be as much as a direct replacement for Expressive Text Actor as possible for cases where you need shadows -- you could also take these concepts and fold them directly into the core class itself. It was also easier to keep separate for shareability without having to deal with patch files or distribution of proprietary code. This is the reason that there is also a direct reference to the included shadow proxy material, as this should make it easier for people not familiar with C++ to just add the files to the right place and be done with it.

### Installation Instructions
As you are working to port the plugin to UE 5.8 [(guide here)](https://github.com/RedCraft86/Random/blob/main/ExpressiveText/UpdateInfo.md), these files can just be included in the plugin directory locations as follows:

1. Add **ExpressiveTextActorShadowed.h** to Source/ExpressiveText/Public/Actors/ExpressiveTextActorShadowed.h
2. Add **ExpressiveTextActorShadowed.cpp** to Source/ExpressiveText/Private/Actors/ExpressiveTextActorShadowed.cpp
3. Add **MAT_WidgetShadowProxy.uasset** to Content/Core/Materials/ActorWidgetMaterials/MAT_WidgetShadowProxy.uasset

The Material was generated for Unreal 5.8.0 and I believe should work in that or newer versions. If you are looking to use this in an older engine version, you can easily re-create this by adding a new material in the editor in the same location:
* Material Domain: Surface
* Blend Mode: Masked
* Shading Model: Unlit
* Two Sided: false
* Add a Texture Parameter 2D node named "RenderTarget", with RGB -> Emissive Color and A -> Opacity Mask
<img width="1154" height="725" alt="material node setup instructions" src="https://github.com/user-attachments/assets/e36bbef7-2b49-498a-ba2d-9c77803f8bfd" />
