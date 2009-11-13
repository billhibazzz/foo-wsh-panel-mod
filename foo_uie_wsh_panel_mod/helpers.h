#pragma once

#include "script_interface.h"

#define TO_VARIANT_BOOL(v) ((v) ? (VARIANT_TRUE) : (VARIANT_FALSE))

namespace helpers
{
	extern bool find_context_command_recur(const char * p_command, pfc::string_base & p_path, contextmenu_node * p_parent, contextmenu_node *& p_out);
	extern bool execute_context_command_by_name(const char * p_name, metadb_handle * p_handle = NULL);
	extern bool execute_mainmenu_command_by_name(const char * p_name);
	extern t_size calc_text_width(const Gdiplus::Font & fn, LPCTSTR text, int len);
	extern bool get_mainmenu_item_checked(const GUID & guid);
	extern void set_mainmenu_item_checked(const GUID & guid, bool checked);

	inline COLORREF convert_argb_to_colorref(DWORD argb)
	{
		return RGB(argb >> RED_SHIFT, argb >> GREEN_SHIFT, argb >> BLUE_SHIFT);
	}

	int int_from_hex_digit(int ch);
	int int_from_hex_byte(const char * hex_byte);

	template<class T>
	inline bool check_gdiplus_object(T * obj)
	{
		return ((obj) && (obj->GetLastStatus() == Gdiplus::Ok));
	}

	const GUID & convert_artid_to_guid(int art_id);
	// bitmap must be NULL
	bool read_album_art_into_bitmap(const album_art_data_ptr & data, Gdiplus::Bitmap ** bitmap);
	HRESULT get_album_art(BSTR rawpath, IGdiBitmap ** pp, int art_id, VARIANT_BOOL need_stub);
	HRESULT get_album_art_embedded(BSTR rawpath, IGdiBitmap ** pp, int art_id);

	// File r/w
	bool read_file(const char * path, pfc::string_base & content);
	// Always save as UTF8 BOM
	bool write_file(const char * path, const pfc::string_base & content);
	class file_info_pairs_filter : public file_info_filter
	{
	public:
		typedef pfc::map_t<pfc::string_simple, pfc::string_simple> t_field_value_map;

	private:
		metadb_handle_ptr m_handle;
		t_field_value_map m_filed_value_map; 
		pfc::avltree_t<pfc::string8, pfc::comparator_stricmp_ascii> m_multivalue_fields;

	public:
		file_info_pairs_filter(const metadb_handle_ptr & p_handle, const t_field_value_map & p_field_value_map, const char * p_multivalue_field = NULL);

		bool apply_filter(metadb_handle_ptr p_location,t_filestats p_stats,file_info & p_info);
	};

	class simple_thread_manager;
	extern simple_thread_manager g_simple_thread_manager;

	// Taken from pfc::thread, with self destruction
	class simple_thread 
	{
	public:
		simple_thread() : m_thread(NULL), m_tid(0) {}

		virtual ~simple_thread() { close(); }

		void start();

		void close();

		inline bool is_active() const
		{
			return (m_thread != NULL);
		}

		inline unsigned get_tid() const
		{
			return m_tid;
		}

	protected:
		virtual void thread_proc() = 0;

	private:
		static unsigned int CALLBACK g_entry(void* p_instance);

		HANDLE m_thread;
		unsigned m_tid;

		PFC_CLASS_NOT_COPYABLE_EX(simple_thread)
	};

	class simple_thread_manager
	{
	public:
		typedef pfc::chain_list_v2_t<simple_thread *> t_thread_list;

		void add(simple_thread * p_thread)
		{
			m_list.add_item(p_thread);
		}

		void remove(simple_thread * p_thread)
		{
			m_list.remove_item(p_thread);
		}

		void remove_all()
		{
			for (t_thread_list::iterator iter = m_list.first(); iter.is_valid(); ++iter)
			{
				(*iter)->close();
			}
		}

	private:
		t_thread_list m_list;
	};

	class album_art_async : public simple_thread
	{
	public:
		struct t_param
		{
			IFbMetadbHandle * handle;
			int art_id;
			IGdiBitmap * bitmap;

			t_param(IFbMetadbHandle * p_handle, int p_art_id, IGdiBitmap * p_bitmap) : handle(p_handle),
				art_id(p_art_id),
				bitmap(p_bitmap)
			{
			}

			virtual ~t_param()
			{
				if (handle)
					handle->Release();

				if (bitmap)
					bitmap->Release();
			}
		};

	private:
		metadb_handle_ptr m_handle;
		_bstr_t m_rawpath;
		int m_art_id;
		VARIANT_BOOL m_need_stub;
		VARIANT_BOOL m_only_embed;
		HWND m_notify_hwnd;

	public:
		album_art_async(HWND notify_hwnd, metadb_handle * handle, int art_id, 
			VARIANT_BOOL need_stub, VARIANT_BOOL only_embed) 
			: m_notify_hwnd(notify_hwnd)
			, m_handle(handle)
			, m_art_id(art_id)
			, m_need_stub(need_stub)
			, m_only_embed(only_embed)
		{
			if (m_handle.is_valid())
				m_rawpath = pfc::stringcvt::string_wide_from_utf8(m_handle->get_path());
		}

		virtual ~album_art_async()
		{ }

	private:
		virtual void thread_proc();
	};
}
