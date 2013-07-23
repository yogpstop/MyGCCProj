void* pBits;
IDirect3DDevice9* g_pd3dDevice;
IDirect3DSurface9* pSurface;
g_pd3dDevice->CreateOffscreenPlainSurface(ScreenWidth, ScreenHeight,
                                          D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, 
                                          &pSurface, NULL);
g_pd3dDevice->GetFrontBufferData(0, pSurface);
D3DLOCKED_RECT lockedRect;
pSurface->LockRect(&lockedRect,NULL,
                   D3DLOCK_NO_DIRTY_UPDATE|
                   D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY)));
for( int i=0 ; i < ScreenHeight ; i++)
{
    memcpy( (BYTE*) pBits + i * ScreenWidth * BITSPERPIXEL / 8 , 
        (BYTE*) lockedRect.pBits + i* lockedRect.Pitch , 
        ScreenWidth * BITSPERPIXEL / 8);
}
g_pSurface->UnlockRect();
pSurface->Release();