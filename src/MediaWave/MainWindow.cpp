#include "pch.h"
#include "pixmaps.h"
#include "PlotWidget.h"
#include "MediumModel.h"
#include "MainWindow.h"

constexpr size_t BufferLen = 32;

MainWindow::MainWindow(MediumModel* model)
    : Fl_Double_Window(700, 500, "MediaWave"), model_(model) {
    create_settings_wnd();

    this->resizable(this);
    this->begin();

    uw = new PlotWidget(10, 10, 480, 235, "u(x)");
    pw = new PlotWidget(10, 255, 480, 235, "p(x)");

    settings_btn = new Fl_Button(550, 20, 90, 25, "Settings");
    step_btn = new Fl_Button(550, 50, 90, 25, "Step");
    start_stop_btn = new Fl_Button(550, 80, 90, 25, "Start");

    bar_plot_check = new Fl_Check_Button(550, 110, 90, 25, "Bar Plot");
    bar_plot_check->callback(switch_bar_plot_cb, this);

    bar_plot_check->value(bar_plot_);

    uw->bar_plot(bar_plot_);
    pw->bar_plot(bar_plot_);

    this->end();

    step_btn->callback(step_cb_st, this);
    start_stop_btn->callback(start_stop_cb_st, this);
    settings_btn->callback(settings_cb_st, this);

    Fl::focus(start_stop_btn);

    uw->view_range(0.0, DefL, -2.0, 2.0);
    uw->ticks(PlotDefaults::TickCount, PlotDefaults::TickSize);

    pw->view_range(0.0, DefL, -2.0, 2.0);
    pw->ticks(PlotDefaults::TickCount, PlotDefaults::TickSize);

    uw->plot(DefN, model->x, model->u1);
    pw->plot(DefN, model->x, model->p1);

    helper_.L = DefL;
    helper_.N = DefN;
    helper_.a = DefA;
    helper_.rho = DefRho;
    helper_.sigma = DefSigma;
    helper_.un_type = DefUNType;
    helper_.pn_type = DefPNType;
    helper_.left_side_type = SideType::Closed;
    helper_.right_side_type = SideType::Closed;
    helper_.bl = 1.0;
    helper_.cl = 0.0;
    helper_.br = 1.0;
    helper_.cr = 0.0;

    helper(helper_);
}

void MainWindow::redraw() {
    uw->plot(model_->N, model_->x, model_->u1);
    pw->plot(model_->N, model_->x, model_->p1);

    uw->redraw();
    pw->redraw();
}

void MainWindow::helper(SettingsHelper h) {
    helper_ = h;

    model_->L = helper_.L;
    model_->N = helper_.N;

    model_->a = helper_.a;
    model_->rho = helper_.rho;
    model_->sigma = helper_.sigma;

    model_->InitFunc(helper_.un_type, helper_.pn_type);

    switch (helper_.left_side_type) {
    case SideType::Closed:
        model_->InitCondL(ClosedBoundary);
        break;
    case SideType::Open:
        model_->InitCondL(OpenBoundary);
        break;
    case SideType::Manual:
        model_->InitCondL({ helper_.bl, helper_.cl });
        break;
    }

    switch (helper_.right_side_type) {
    case SideType::Closed:
        model_->InitCondR(ClosedBoundary);
        break;
    case SideType::Open:
        model_->InitCondR(OpenBoundary);
        break;
    case SideType::Manual:
        model_->InitCondR({ helper_.br, helper_.cr });
        break;
    }

    uw->view_range(0.0, model_->L, -2.0, 2.0);
    uw->ticks(PlotDefaults::TickCount, PlotDefaults::TickSize);

    pw->view_range(0.0, model_->L, -2.0, 2.0);
    pw->ticks(PlotDefaults::TickCount, PlotDefaults::TickSize);

    uw->redraw();
    pw->redraw();

    this->model_->Reset();
    this->redraw();
}

SettingsHelper MainWindow::helper() {
    return this->helper_;
}

void MainWindow::step_cb_st(Fl_Widget*, void* v) {
    auto w = static_cast<MainWindow*>(v);
    w->step_cb();
}

void MainWindow::step_cb() {
    this->model_->Step();
    this->redraw();
}

void MainWindow::start_stop_cb_st(Fl_Widget*, void* v) {
    auto w = static_cast<MainWindow*>(v);
    w->start_stop_cb();
}

void MainWindow::start_stop_cb() {
    if (running_) {
        running_ = false;

        this->settings_btn->activate();
        this->step_btn->activate();

        this->start_stop_btn->label("Start");

        Fl::remove_timeout(timer_cb_st, this);
    }
    else {
        running_ = true;

        this->settings_btn->deactivate();
        this->step_btn->deactivate();

        this->start_stop_btn->label("Stop");

        Fl::add_timeout(TimerInterval, timer_cb_st, this);
    }
}

void MainWindow::settings_cb_st(Fl_Widget*, void* v) {
    auto w = static_cast<MainWindow*>(v);
    w->settings_cb();
}

void MainWindow::settings_cb() {
    this->set_inputs(helper_);
    settings_wnd->show();
}

void MainWindow::timer_cb_st(void* v) {
    auto w = static_cast<MainWindow*>(v);
    w->timer_cb();
}

void MainWindow::timer_cb() {
    this->model_->Step();
    this->redraw();

    Fl::repeat_timeout(TimerInterval, timer_cb_st, this);
}

// Settings window
const char label_close_left[] = "[==";
const char label_close_right[] = "==]";
const char label_open[] = "===";
const char label_manual[] = "~~~";

void MainWindow::create_side_selector(Fl_Tree* t) {
    t->showroot(0);
    t->connectorstyle(FL_TREE_CONNECTOR_NONE);
    t->selectmode(FL_TREE_SELECT_SINGLE);
    t->connectorwidth(0);
    t->marginleft(0);
    t->visible_focus(0);
}

void MainWindow::set_tree_icons(Fl_Tree* t) {
    static Fl_Pixmap L_close_l(L_close_l_xpm);
    static Fl_Pixmap L_close_r(L_close_r_xpm);
    static Fl_Pixmap L_open(L_open_xpm);
    static Fl_Pixmap L_manual(L_manual_xpm);

    for (Fl_Tree_Item* item = t->first(); item; item = item->next()) {
        if (strcmp(item->label(), label_close_left) == 0)
            item->usericon(&L_close_l);
        else if (strcmp(item->label(), label_open) == 0)
            item->usericon(&L_open);
        else if (strcmp(item->label(), label_close_right) == 0)
            item->usericon(&L_close_r);
        else if (strcmp(item->label(), label_manual) == 0)
            item->usericon(&L_manual);

        item->label("");
    }
}

void MainWindow::select_side_type(Fl_Tree* t, SideType type) {
    for (Fl_Tree_Item* item = t->first(); item; item = item->next()) {
        auto itemType = static_cast<SideType>(reinterpret_cast<uintptr_t>(item->user_data()));
        if (itemType == type) {
            t->select_only(item);
            return;
        }
    }
}

void MainWindow::create_settings_wnd() {
    settings_wnd = new Fl_Window(320, 480, "Settings");

    settings_wnd->begin();

    len_in = new Fl_Input(70, 10, 90, 25, "L = ");
    n_in = new Fl_Input(70, 40, 90, 25, "N = ");
    a_in = new Fl_Input(70, 80, 90, 25, "a = ");
    rho_in = new Fl_Input(70, 110, 90, 25, "rho = ");
    sigma_in = new Fl_Input(70, 140, 90, 25, "sigma = ");

    u_choice = new Fl_Choice(220, 10, 90, 25, "U(x)");
    u_choice->add("- - -");
    u_choice->add("^ - ^");
    u_choice->add("^ - -");
    u_choice->add("- ^ -");
    u_choice->add("- - ^");
    u_choice->add(" V ^ ");
    u_choice->add(" ^ V ");

    p_choice = new Fl_Choice(220, 40, 90, 25, "P(x)");
    p_choice->add("- - -");
    p_choice->add("^ - ^");
    p_choice->add("^ - -");
    p_choice->add("- ^ -");
    p_choice->add("- - ^");

    left_side_choice = new Fl_Tree(90, 200, 52, 116, "Left");
    create_side_selector(left_side_choice);
    left_side_choice->clear();
    left_side_choice->add(label_close_left)->user_data((void*)(SideType::Closed));
    left_side_choice->add(label_open)->user_data((void*)(SideType::Open));
    left_side_choice->add(label_manual)->user_data((void*)(SideType::Manual));
    left_side_choice->selection_color(FL_DARK2);
    set_tree_icons(left_side_choice);
    left_side_choice->callback(left_side_cb_st, this);
    bl_in = new Fl_Input(70, 330, 90, 25, "bl");
    cl_in = new Fl_Input(70, 360, 90, 25, "cl");

    right_side_choice = new Fl_Tree(210, 200, 52, 116, "Right");
    create_side_selector(right_side_choice);
    right_side_choice->clear();
    right_side_choice->add(label_close_right)->user_data((void*)(SideType::Closed));
    right_side_choice->add(label_open)->user_data((void*)(SideType::Open));
    right_side_choice->add(label_manual)->user_data((void*)(SideType::Manual));
    right_side_choice->selection_color(FL_DARK2);
    set_tree_icons(right_side_choice);
    right_side_choice->callback(right_side_cb_st, this);
    br_in = new Fl_Input(190, 330, 90, 25, "br");
    cr_in = new Fl_Input(190, 360, 90, 25, "cr");

    apply_btn = new Fl_Button(155, 450, 75, 23, "Apply");
    close_btn = new Fl_Button(235, 450, 75, 23, "Close");

    apply_btn->callback(apply_cb_st, this);
    close_btn->callback(close_cb_st, this);

    settings_wnd->end();
    settings_wnd->set_modal();
}

void MainWindow::set_inputs(SettingsHelper helper) {
    char str[BufferLen] = { 0 };

    u_choice->value(helper.un_type);
    p_choice->value(helper.pn_type);
    set_lcond_inputs(helper);
    set_rcond_inputs(helper);

    snprintf(str, BufferLen, "%.4f", helper.L);
    len_in->value(str);

    snprintf(str, BufferLen, "%d", helper.N);
    n_in->value(str);

    snprintf(str, BufferLen, "%.4f", helper.a);
    a_in->value(str);

    snprintf(str, BufferLen, "%.4f", helper.rho);
    rho_in->value(str);

    snprintf(str, BufferLen, "%.4f", helper.sigma);
    sigma_in->value(str);
}

SettingsHelper MainWindow::get_inputs() {
    char str[BufferLen] = { 0 };
    SettingsHelper helper;

    helper.un_type = u_choice->value();
    helper.pn_type = p_choice->value();
    get_lcond_inputs(helper);
    get_rcond_inputs(helper);

    strncpy(str, len_in->value(), BufferLen);
    helper.L = atof(str);

    strncpy(str, n_in->value(), BufferLen);
    helper.N = atoi(str);

    strncpy(str, a_in->value(), BufferLen);
    helper.a = atof(str);

    strncpy(str, rho_in->value(), BufferLen);
    helper.rho = atof(str);

    strncpy(str, sigma_in->value(), BufferLen);
    helper.sigma = atof(str);

    return helper;
}

void MainWindow::set_lcond_inputs(SettingsHelper helper) {
    char str[BufferLen] = { 0 };
    select_side_type(left_side_choice, helper.left_side_type);

    snprintf(str, BufferLen, "%.1f", helper.bl);
    bl_in->value(str);

    snprintf(str, BufferLen, "%.1f", helper.cl);
    cl_in->value(str);
}

void MainWindow::get_lcond_inputs(SettingsHelper& helper) {
    char str[BufferLen] = { 0 };
    Fl_Tree_Item* item = left_side_choice->first_selected_item();
    helper.left_side_type = static_cast<SideType>(reinterpret_cast<uintptr_t>(item->user_data()));

    if (helper.left_side_type == SideType::Manual) {
        strncpy(str, bl_in->value(), BufferLen);
        helper.bl = atof(str);

        strncpy(str, cl_in->value(), BufferLen);
        helper.cl = atof(str);

    }
    else if (helper.left_side_type == SideType::Closed) {
        helper.cl = 0.0;
        helper.bl = 1.0;

    }
    else if (helper.left_side_type == SideType::Open) {
        helper.cl = 0.0;
        helper.bl = 1.0;
    }
}

void MainWindow::set_rcond_inputs(SettingsHelper helper) {
    char str[BufferLen] = { 0 };
    select_side_type(right_side_choice, helper.right_side_type);

    snprintf(str, BufferLen, "%.1f", helper.br);
    br_in->value(str);

    snprintf(str, BufferLen, "%.1f", helper.cr);
    cr_in->value(str);
}

void MainWindow::get_rcond_inputs(SettingsHelper& helper) {
    char str[BufferLen] = { 0 };
    Fl_Tree_Item* item = right_side_choice->first_selected_item();
    helper.right_side_type = static_cast<SideType>(reinterpret_cast<uintptr_t>(item->user_data()));

    if (helper.right_side_type == SideType::Manual) {
        strncpy(str, br_in->value(), BufferLen);
        helper.br = atof(str);

        strncpy(str, cr_in->value(), BufferLen);
        helper.cr = atof(str);

    }
    else if (helper.right_side_type == SideType::Closed) {
        helper.cr = ClosedBoundary.c;
        helper.br = ClosedBoundary.b;

    }
    else if (helper.right_side_type == SideType::Open) {
        helper.cr = OpenBoundary.c;
        helper.br = OpenBoundary.b;
    }
}

void MainWindow::close_cb_st(Fl_Widget*, void* v) {
    auto w = static_cast<MainWindow*>(v);
    w->close_cb();
}

void MainWindow::close_cb() {
    settings_wnd->hide();
}

void MainWindow::apply_cb_st(Fl_Widget*, void* v) {
    MainWindow* w = (MainWindow*)v;
    w->apply_cb();
}

void MainWindow::apply_cb() {
    settings_wnd->hide();
    this->helper(this->get_inputs());
}

void MainWindow::left_side_cb_st(Fl_Widget*, void* v) {
    auto w = static_cast<MainWindow*>(v);
    w->left_side_cb();
}

void MainWindow::left_side_cb() {
    Fl_Tree_Item* item = left_side_choice->callback_item();
    Fl_Tree_Reason reason = left_side_choice->callback_reason();
    SideType v = static_cast<SideType>(reinterpret_cast<uintptr_t>(item->user_data()));

    if (v == SideType::Manual && reason == FL_TREE_REASON_SELECTED) {
        bl_in->show();
        cl_in->show();
    }
    else {
        bl_in->hide();
        cl_in->hide();
    }
}

void MainWindow::right_side_cb_st(Fl_Widget*, void* v) {
    auto w = static_cast<MainWindow*>(v);
    w->right_side_cb();
}

void MainWindow::right_side_cb() {
    Fl_Tree_Item* item = right_side_choice->callback_item();
    Fl_Tree_Reason reason = right_side_choice->callback_reason();
    SideType v = static_cast<SideType>(reinterpret_cast<uintptr_t>(item->user_data()));

    if (v == SideType::Manual && reason == FL_TREE_REASON_SELECTED) {
        br_in->show();
        cr_in->show();
    }
    else {
        br_in->hide();
        cr_in->hide();
    }
}

void MainWindow::switch_bar_plot_cb(Fl_Widget*, void* v) {
    auto w = static_cast<MainWindow*>(v);
    w->switch_bar_plot();
}

void MainWindow::switch_bar_plot() {
    bar_plot_ = !bar_plot_;

    uw->bar_plot(bar_plot_);
    uw->redraw();

    pw->bar_plot(bar_plot_);
    pw->redraw();
}
