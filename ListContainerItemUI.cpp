#include "StdAfx.h"
#include "ListContainerItemUI.h"

CListContainerItemUI::CListContainerItemUI(void)
{
}

CListContainerItemUI::~CListContainerItemUI(void)
{
}


LPCTSTR CListContainerItemUI::GetClass() const
{
	return _T("ListContainerItem");
}


LPVOID CListContainerItemUI::GetInterface( LPCTSTR pstrName )
{
	if( _tcscmp(pstrName, DUI_CTR_LISTITEM) == 0 ) return static_cast<IListItemUI*>(this);
	if( _tcscmp(pstrName, _T("ListContainerItem")) == 0 ) return static_cast<CListContainerItemUI*>(this);
	return CControlUI::GetInterface(pstrName);
}


void CListContainerItemUI::DoPaint( HDC hDC, const RECT& rcPaint )
{
	DrawItemBk(hDC,rcPaint);
	CContainerUI::Paint(hDC,rcPaint);	//重载画背景
}

void CListContainerItemUI::DoEvent( TEventUI& event )
{
	DoItemEvent(event);
	CContainerUI::DoEvent(event);
}

int CListContainerItemUI::GetIndex() const
{
	return m_iIndex;
}

void CListContainerItemUI::SetIndex( int iIndex )
{
	m_iIndex = iIndex;
}

IListOwnerUI* CListContainerItemUI::GetOwner()
{
	return m_pOwner;
}

void CListContainerItemUI::SetOwner( CControlUI* pOwner )
{
	m_pOwner = static_cast<IListOwnerUI*>(pOwner->GetInterface(_T("IListOwner")));
}

bool CListContainerItemUI::IsSelected() const
{
	return m_bSelected;
}

bool CListContainerItemUI::Select( bool bSelect /*= true*/, bool bTriggerEvent/*=true*/ )
{
	if( !IsEnabled() ) return false;
	if( bSelect == m_bSelected ) return true;
	m_bSelected = bSelect;
	if( bSelect && m_pOwner != NULL ) m_pOwner->SelectItem(m_iIndex, bTriggerEvent);
	Invalidate();
	return true;
}

bool CListContainerItemUI::IsExpanded() const
{
	return false;
}

bool CListContainerItemUI::Expand( bool bExpand /*= true*/ )
{
	return false;
}

void CListContainerItemUI::DrawItemText( HDC hDC, const RECT& rcItem )
{
	//什么都不做
}

void CListContainerItemUI::DrawItemBk( HDC hDC, const RECT& rcItem )
{
	ASSERT(m_pOwner);
	if( m_pOwner == NULL ) return;
	TListInfoUI* pInfo = m_pOwner->GetListInfo();
	if( pInfo == NULL ) return;
	DWORD iBackColor = 0;
	if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) iBackColor = pInfo->dwBkColor;
	if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		iBackColor = pInfo->dwHotBkColor;
	}
	if( IsSelected() ) {
		iBackColor = pInfo->dwSelectedBkColor;
	}
	if( !IsEnabled() ) {
		iBackColor = pInfo->dwDisabledBkColor;
	}

	if ( iBackColor != 0 ) {
		CRenderEngine::DrawColor(hDC, rcItem, GetAdjustColor(iBackColor));
	}

	if( !IsEnabled() ) {
		if( DrawImage(hDC, pInfo->diDisabled) ) return;
	}
	if( IsSelected() ) {
		if( DrawImage(hDC, pInfo->diSelected) ) return;
	}
	if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		if( DrawImage(hDC, pInfo->diHot) ) return;
	}

	if( !DrawImage(hDC, m_diBk) ) {
		if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) {
			if( DrawImage(hDC, pInfo->diBk) ) return;
		}
	}

	if ( pInfo->dwLineColor != 0 ) {
		RECT rcLine = { rcItem.left, rcItem.bottom - 1, rcItem.right, rcItem.bottom - 1 };
		CRenderEngine::DrawLine(hDC, rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
	}
}

void CListContainerItemUI::DoItemEvent( TEventUI& event )
{
	//选中
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
		else CContainerUI::DoEvent(event);
		return;
	}

	if( event.Type == UIEVENT_DBLCLICK )
	{
		if( IsEnabled() ) {
			Activate();
			Invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_KEYDOWN && IsEnabled() )
	{
		if( event.chKey == VK_RETURN ) {
			Activate();
			Invalidate();
			return;
		}
	}
	// An important twist: The list-item will send the event not to its immediate
	// parent but to the "attached" list. A list may actually embed several components
	// in its path to the item, but key-presses etc. needs to go to the actual list.
	if( m_pOwner != NULL ) m_pOwner->DoEvent(event); else CContainerUI::DoEvent(event);
}

void CListContainerItemUI::Invalidata()
{
	if( !IsVisible() ) return;

	if( GetParent() ) {
		CContainerUI* pParentContainer = static_cast<CContainerUI*>(GetParent()->GetInterface(_T("Container")));
		if( pParentContainer ) {
			RECT rc = pParentContainer->GetPos();
			RECT rcInset = pParentContainer->GetInset();
			rc.left += rcInset.left;
			rc.top += rcInset.top;
			rc.right -= rcInset.right;
			rc.bottom -= rcInset.bottom;
			CScrollBarUI* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
			if( pVerticalScrollBar && pVerticalScrollBar->IsVisible() ) rc.right -= pVerticalScrollBar->GetFixedWidth();
			CScrollBarUI* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
			if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

			RECT invalidateRc = m_rcItem;
			if( !::IntersectRect(&invalidateRc, &m_rcItem, &rc) ) 
			{
				return;
			}

			CControlUI* pParent = GetParent();
			RECT rcTemp;
			RECT rcParent;
			while( pParent = pParent->GetParent() )
			{
				rcTemp = invalidateRc;
				rcParent = pParent->GetPos();
				if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
				{
					return;
				}
			}

			if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
		}
		else {
			CControlUI::Invalidate();
		}
	}
	else {
		CControlUI::Invalidate();
	}
}

bool CListContainerItemUI::Activate()
{
	if( !CControlUI::Activate() ) return false;
	if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMACTIVATE);
	return true;
}
