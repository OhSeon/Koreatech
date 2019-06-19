
// MFCApplication3View.cpp : CMFCApplication3View Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "MFCApplication3.h"
#endif

#include "mythread.h"

#include "MFCApplication3Doc.h"
#include "MFCApplication3View.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCApplication3View

IMPLEMENT_DYNCREATE(CMFCApplication3View, CView)

BEGIN_MESSAGE_MAP(CMFCApplication3View, CView)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CMFCApplication3View ����/�Ҹ�

CMFCApplication3View::CMFCApplication3View()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	m_bView = false;

}

CMFCApplication3View::~CMFCApplication3View()
{
}

BOOL CMFCApplication3View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CMFCApplication3View �׸���




// CMFCApplication3View ����

#ifdef _DEBUG
void CMFCApplication3View::AssertValid() const
{
	CView::AssertValid();
}

void CMFCApplication3View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCApplication3Doc* CMFCApplication3View::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplication3Doc)));
	return (CMFCApplication3Doc*)m_pDocument;
}
#endif //_DEBUG

void CMFCApplication3View::Wait(DWORD dwMillisecond)
{
	MSG msg;
	DWORD dwStart;
	dwStart = GetTickCount();

	while (GetTickCount() - dwStart < dwMillisecond)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}
// CMFCApplication3View �޽��� ó����



void CMFCApplication3View::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CClientDC dc(this); // device context for painting

	CView::OnLButtonDown(nFlags, point);
	CMFCApplication3Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);


	struct shm_remove
	{
		shm_remove() { shared_memory_object::remove("ODSharedMemory"); }
		~shm_remove() { shared_memory_object::remove("ODSharedMemory"); }
	} remover;

	//declare var
	ti t;

	do {
		t.cam.vc.open("EXID.mp4");
		std::cout << "Try VideoCapture Open" << std::endl << std::endl;
	} while (!t.cam.vc.isOpened());

	t.cam.vc >> t.cam.image;

	//parameter check
	int nchs = t.cam.image.channels();
	std::cout << "\tchannel:" << nchs << std::endl;
	if (nchs != 3 && nchs != 4) {
		t.cam.vc.release();
		getchar();
		return;
	}

	int dsz = t.cam.image.rows * t.cam.image.step;

	//Create a new segment with given name and size
	managed_shared_memory segment(create_only, "ODSharedMemory", dsz * 20);

	//allocation 
	t.od = (odi*)segment.allocate(sizeof(odi));

	assert(t.od != NULL);

	//set image size
	t.od->dsz = dsz;
	t.od->iHeight = t.cam.image.size().height;
	t.od->iWidth = t.cam.image.size().width;

	std::cout << " od dsz   : " << t.od->dsz << std::endl;

	// check allocation , SIB
	assert(&t.od->SIBimage != NULL);

	//SIB allocation, get handle
	for (int i = 0; i < 2; i++) {
		t.od->SIBimage[i]._pMem = segment.allocate(dsz * 3);
		assert(&t.od->SIBimage[i]._pMem != NULL);
		t.od->SIBimage[i]._hMem = segment.get_handle_from_address(t.od->SIBimage[i]._pMem);
		assert(&t.od->SIBimage[i]._hMem != NULL);
	}
	//initial data copy
	memcpy(t.od->SIBimage[0]._pMem, t.cam.image.data, dsz);
	memcpy(t.od->SIBimage[1]._pMem, t.cam.image.data, dsz);

	// get handle
	managed_shared_memory::handle_t handle = segment.get_handle_from_address(t.od);

	//execute
	std::stringstream s;
	char odc_fn[] = "MFCodclient.exe";
	s << odc_fn << " " << handle << " " << t.od << std::ends;
	std::cout << "Main cmd: " << s.str() << std::endl;

	t.od->nDetections = 0;
	t.od->init_flag = false;
	t.od->index = 0;
	t.od->frame_full = false;
	t.od->ready_flag = false;
	t.od->run_flag = false;
	//RUN
	std::cout << "\t\tinitialize od\n";
	std::thread p1(program_thread, std::ref(s));

	while (!t.od->init_flag) boost::detail::Sleep(100);

	std::thread p2(_camera, std::ref(t));

#define person
#define object
		while (1) {
			image = cv::Mat(t.cam.image);
			if (t.od->run_flag) {
				for (int i = 0; i < t.od->nDetections; i++) {
					std::cout
						<< "classes: " << t.od->classes[i] << "\t"
						<< "scores : " << t.od->scores[i] << "\t"
						<< "boxes  : " << t.od->boxes[i].x << "\t" << t.od->boxes[i].y << "\t"
						<< t.od->boxes[i].width << "\t" << t.od->boxes[i].height
						<< std::endl;
#ifdef person
					if (t.od->classes[i] == 1) {
						cv::rectangle(image, t.od->boxes[i], cv::Scalar(0, 0, 255), 2);
					}
#endif
#ifdef object 
					if (t.od->classes[i] < 70 && t.od->classes[i] != 1) {
						cv::rectangle(image, t.od->boxes[i], cv::Scalar(0, 0, 255), 2);
					}
#endif
				}

				t.od->run_flag = false;
			}
			else {
				boost::detail::Sleep(50);
			}

			result = image.clone();
			//imshow("result", result);

			//t.cam.vc >> t.cam.image;
			//frame = cv::Mat(t.cam.image);
			frame = cv::Mat(result);
			if (frame.empty())
				return;

			CRect Bound(50, 50, 350, 700);

			BITMAPINFOHEADER bi = { sizeof(bi) };
			bi.biWidth = frame.cols;
			bi.biHeight = -frame.rows;
			bi.biBitCount = (WORD)(frame.channels() * 8);
			bi.biPlanes = 1;

			StretchDIBits(dc, 50, 50, Bound.right, Bound.bottom, 0, 0, frame.cols, frame.rows, frame.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS, SRCCOPY);
			//waitKey(20);
			this->Invalidate(false);
			Wait(20);
		}
	
	p1.join();
	p2.join();

	segment.deallocate(t.od->SIBimage[0]._pMem);
	segment.deallocate(t.od->SIBimage[1]._pMem);
	segment.deallocate(t.od);

	std::cout << " Main End !" << std::endl;
	t.cam.vc.release();
}


void CMFCApplication3View::OnDraw(CDC* /*pDC*/)
{
	CMFCApplication3Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
}


