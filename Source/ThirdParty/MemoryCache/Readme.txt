How to incorporate into the Urho solution:

1. Add project to solution
2. Update Urho3D project dependencies to include the MemoryCache proj
3. Add the MemoryCache.lib to the Urho3D linker input
4. Create a folder junction link (mklink /J) into the Urho3D\include\Urho3D\ThirdParty folder

Good luck!